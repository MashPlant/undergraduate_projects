use crate::{load_pinyin, gbk2string, predict, TrainPiece, WordInfo};
use crate::PUNCTUATION;
use rayon::prelude::*;
use std::path::Path;
use std::io::*;
use std::str::from_utf8;
use std::fs::File;
use std::collections::HashMap;

#[allow(unused)]
pub fn make() -> Result<()> {
  let py2ch = load_pinyin("拼音汉字表.txt")?;
  let mut ch2py = HashMap::new();
  for (py, chs) in &py2ch {
    for &ch in chs {
      ch2py.insert(ch, py.as_str());
    }
  }
  let rd = BufReader::new(File::open(Path::new("test").join("test.txt"))?);
  let (mut wt_in, mut wt_out) = (BufWriter::new(File::create(Path::new("test").join("input.txt"))?), File::create(Path::new("test").join("output.txt"))?);
  for ln in rd.split(b'\n').map(|l| l.unwrap()) {
    let s = gbk2string(&ln);
    let tp = serde_json::from_str::<TrainPiece>(&s).unwrap();
    for piece in tp.html.split(|ch| PUNCTUATION.contains(&ch)) {
      if piece.chars().count() < 5 || piece.chars().any(|ch| !ch2py.contains_key(&ch)) {
        continue;
      }
      for ch in piece.chars() {
        write!(wt_in, "{} ", ch2py.get(&ch).unwrap())?;
        write!(wt_out, "{}", ch)?;
      }
      wt_in.write_all(&[b'\n'])?;
      wt_out.write_all(&[b'\n'])?;
    }
  }
  Ok(())
}

#[derive(Debug)]
pub struct TestRes {
  n_ch: usize,
  n_sen: usize,
  ch_acc: f64,
  sen_acc: f64,
}

#[allow(unused)]
pub fn rate(info: &WordInfo, pinyin: &HashMap<String, Vec<char>>) -> Result<TestRes> {
  let req = File::open(Path::new("test").join("input.txt"))?;
  let ans = File::open(Path::new("test").join("output.txt"))?;
  let mut req = BufReader::new(req).split(b'\n').map(|l| (l.unwrap(), String::new())).collect::<Vec<_>>();
  let mut ans = BufReader::new(ans).split(b'\n').map(|l| (l.unwrap(), String::new())).collect::<Vec<_>>();
  req.par_iter_mut().for_each(|(ln, s)| {
    *s = predict(info, pinyin, &from_utf8(ln).unwrap().split_whitespace().collect::<Vec<_>>()).unwrap();
  });
  ans.par_iter_mut().for_each(|(ln, s)| *s = from_utf8(ln).unwrap().to_string());
  let (ok_ch, n_ch, ok_sen) = req.par_iter().zip(ans.par_iter()).map(|((_, pred), (_, ans))| {
    let (mut ok_ch, mut n_ch) = (0, 0);
    for (ch1, ch2) in pred.chars().zip(ans.chars()) {
      n_ch += 1;
      if ch1 == ch2 { ok_ch += 1; }
    }
    (ok_ch, n_ch, if ok_ch == n_ch { 1 } else { 0 })
  }).reduce(|| (0, 0, 0), |(ok_ch1, n_ch1, ok_sen1), (ok_ch2, n_ch2, ok_sen2)| (ok_ch1 + ok_ch2, n_ch1 + n_ch2, ok_sen1 + ok_sen2));
  let mut out = BufWriter::new(File::create(Path::new("test").join("predict.txt"))?);
  for (_, pred) in &req {
    write!(out, "{}\n", pred)?;
  }
  Ok(TestRes { n_ch, n_sen: ans.len(), ch_acc: ok_ch as f64 / n_ch as f64, sen_acc: ok_sen as f64 / ans.len() as f64 })
}