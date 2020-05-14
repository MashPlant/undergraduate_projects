extern crate serde;
extern crate serde_json;
extern crate encoding;
extern crate rayon;
extern crate bincode;
#[macro_use]
extern crate lazy_static;
extern crate clap;
extern crate rustyline;

use serde::{Serialize, Deserialize};
use std::fs::{self, File};
use std::io::{self, prelude::*, BufReader, BufWriter};
use std::collections::{HashMap, HashSet};

mod test;

lazy_static! {
  // PUNCTUATION cannot be ch1 in (ch1, ch2)
  static ref PUNCTUATION: HashSet<char> = {
    let mut s = HashSet::new();
    for &ch in &['，', ',', '。', '.', '；', ';', '（', '(', '）', ')', '“', '"', '”', '！', '!',
              '、', '：', ':', '？', '?', '《', '》', '【', '[', '】', ']'] {
      s.insert(ch);
    }
    s
  };
}

// STOP_WORD contains PUNCTUATION, it is to judge whether a ch is suitable for the end of a sentence
static STOP_WORD: &'static [char] = &['的', '了', '在', '是', '我', '有', '和', '就',
  '不', '人', '都', '一', '个', '上', '也', '很', '到', '说', '要', '去', '你',
  '会', '着', '没', '有', '看', '好', '这', '他', '她', '它',
  '，', ',', '。', '.', '；', ';', '（', '(', '）', ')', '“', '"', '”', '！', '!',
  '、', '：', ':', '？', '?', '《', '》', '【', '[', '】', ']'];

#[derive(Serialize, Deserialize)]
pub struct WordInfo {
  // char -> ln(appear times); not exist -> neg inf
  u_map: HashMap<char, f32>,
  // (char, char) -> ln(appear times); not exist -> neg inf
  b_map: HashMap<(char, char), f32>,
  // char -> last_correction factor; not exist -> 0
  last_correction: HashMap<char, f32>,
}

#[derive(Deserialize, Debug)]
pub struct TrainPiece {
  html: String,
  time: String,
  url: String,
}

fn gbk2string(gbk: &[u8]) -> String {
  use encoding::{all::GBK, Encoding, DecoderTrap};

  let mut s = GBK.decode(&gbk, DecoderTrap::Strict).unwrap();
  if s.ends_with('\r') { s.pop(); }
  s
}

fn load_train(cache_path: &str, train_path: &str) -> io::Result<WordInfo> {
  fn try_load_cache(path: &str) -> io::Result<WordInfo> {
    let mut f = File::open(path)?;
    let mut v = Vec::new();
    f.read_to_end(&mut v)?;
    Ok(bincode::deserialize(&v).unwrap())
  }

  match try_load_cache(cache_path) {
    Ok(ok) => Ok(ok),
    Err(_) => {
      let cache = do_load_train(train_path)?;
      let mut f = File::create(cache_path)?;
      f.write_all(&bincode::serialize(&cache).unwrap())?;
      Ok(cache)
    }
  }
}

fn do_load_train(path: &str) -> io::Result<WordInfo> {
  use rayon::prelude::*;

  let mut fs = Vec::new();
  for p in fs::read_dir(path)?.collect::<Vec<_>>() {
    fs.push(File::open(p?.path())?);
  }
  let (u_map, b_map) = fs.into_par_iter().map(|f| {
    let rd = BufReader::new(&f);
    let (mut u_map, mut b_map) = (HashMap::new(), HashMap::new());
    for ln in rd.split(b'\n').map(|l| l.unwrap()) {
      let s = gbk2string(&ln);
      let tp = serde_json::from_str::<TrainPiece>(&s).unwrap();
      for (ch1, ch2) in tp.html.chars().zip(tp.html.chars().skip(1)) {
        if PUNCTUATION.contains(&ch1) { continue; }
        *u_map.entry(ch1).or_insert_with(|| 0u32) += 1;
        *b_map.entry((ch1, ch2)).or_insert_with(|| 0u32) += 1;
      }
    }
    (u_map, b_map)
  }).reduce(|| (HashMap::new(), HashMap::new()), |(l_u_map, l_b_map), (r_u_map, r_b_map)| {
    use std::hash::Hash;

    fn merge<K: Eq + Hash>(mut a: HashMap<K, u32>, mut b: HashMap<K, u32>) -> HashMap<K, u32> {
      if a.len() < b.len() {
        for (k, v) in a {
          *b.entry(k).or_insert_with(|| 0) += v;
        }
        b
      } else {
        for (k, v) in b {
          *a.entry(k).or_insert_with(|| 0) += v;
        }
        a
      }
    }
    (merge(l_u_map, r_u_map), merge(l_b_map, r_b_map))
  });
  let last_correction = u_map.iter().map(|(&k, &v)| {
    let mut sum = 0;
    for &sw in STOP_WORD.iter() {
      sum += b_map.get(&(k, sw)).map(|&x| x).unwrap_or(0);
    }
    (k, (1.0 + sum as f32 / v as f32).ln())
  }).collect::<HashMap<_, _>>();
  let u_map = u_map.into_iter().map(|(k, v)| (k, (v as f32).ln())).collect::<HashMap<_, _>>();
  let b_map = b_map.into_iter().map(|(k, v)| (k, (v as f32).ln())).collect::<HashMap<_, _>>();
  Ok(WordInfo { u_map, b_map, last_correction })
}

fn load_pinyin(path: &str) -> io::Result<HashMap<String, Vec<char>>> {
  let f = File::open(path)?;
  let rd = BufReader::new(&f);
  let mut ret = HashMap::new();
  for ln in rd.split(b'\n').map(|l| l.unwrap()) {
    let s = gbk2string(&ln);
    let py = s.split(' ').nth(0).unwrap().to_owned();
    let chs = ret.entry(py).or_insert_with(|| Vec::new());
    for ch in s.split(' ').skip(1) {
      chs.push(ch.chars().nth(0).unwrap());
    }
  }
  Ok(ret)
}

#[allow(unused)]
fn predict<'a>(info: &WordInfo, pinyin: &HashMap<String, Vec<char>>, ss: &[&'a str]) -> Result<String, &'a str> {
  use std::f32;

  let WordInfo { u_map, b_map, last_correction } = info;
  let mut dp = vec![Vec::new(); ss.len()];
  for (x, &s) in dp.iter_mut().zip(ss.iter()) {
    match pinyin.get(s) {
      Some(v) => *x = vec![(f32::NEG_INFINITY, 0); v.len()],
      None => return Err(s),
    }
  }
  // and then we can safely call unwrap for pinyin.get(s)
  {
    let first = pinyin.get(ss[0]).unwrap();
    for (i, x) in dp[0].iter_mut().enumerate() {
      x.0 = *u_map.get(&first[i]).unwrap_or(&f32::NEG_INFINITY);
    }
  }
  for i in 0..dp.len() - 1 {
    let cur = pinyin.get(ss[i]).unwrap();
    let nxt = pinyin.get(ss[i + 1]).unwrap();
    for j in 0..dp[i].len() {
      let (ch, val) = (cur[j], dp[i][j].0);
      if val == f32::NEG_INFINITY { continue; }
      let inv_freq = -*u_map.get(&ch).unwrap(); // must exist, because val != neg inf
      for (k, upd) in dp[i + 1].iter_mut().enumerate() {
        let x = val + *b_map.get(&(ch, nxt[k])).unwrap_or(&f32::NEG_INFINITY) + inv_freq;
        if upd.0 < x { *upd = (x, j as u32); }
      }
    }
  }
  {
    let last = pinyin.get(ss[ss.len() - 1]).unwrap();
    for (i, x) in dp.last_mut().unwrap().iter_mut().enumerate() {
      x.0 += 2.0 * *last_correction.get(&last[i]).unwrap_or(&0.0);
    }
  }
  let (mut max, mut arg) = (f32::NEG_INFINITY, 0);
  for (i, x) in dp[dp.len() - 1].iter().enumerate() {
    if max < x.0 {
      (max = x.0, arg = i);
    }
  }
  let mut ret = String::new();
  for (i, ans) in dp.iter().enumerate().rev() {
    ret.push(pinyin.get(ss[i]).unwrap()[arg]);
    arg = ans[arg].1 as usize;
  }
  Ok(ret.chars().rev().collect())
}

fn main() {
  use clap::{Arg, App, ArgMatches};
  use rustyline::Editor;

  let cmd = App::new("Pinyin to Chinese Character")
    .author("MashPlant <li-ch17@mails.tsinghua.edu.cn>")
    .arg(Arg::with_name("INPUT"))
    .arg(Arg::with_name("OUTPUT").short("o").long("output").value_name("FILE").takes_value(true))
    .arg(Arg::with_name("TRAIN").short("t").long("train").value_name("FILE").takes_value(true))
    .arg(Arg::with_name("CACHE").short("c").long("cache").value_name("FILE").takes_value(true))
    .arg(Arg::with_name("PINYIN").short("p").long("pinyin").value_name("FILE").takes_value(true))
    .get_matches()
    ;

  fn inner(cmd: &ArgMatches) -> io::Result<()> {
    let info = load_train(
      cmd.value_of("CACHE").unwrap_or("cache.bin"),
      cmd.value_of("TRAIN").unwrap_or("sina_news"),
    )?;
    let pinyin = load_pinyin(cmd.value_of("PINYIN").unwrap_or("拼音汉字表.txt"))?;
    let mut output: Box<io::Write> = if let Some(output) = cmd.value_of("OUTPUT") {
      let out = File::create(output)?;
      Box::new(BufWriter::new(out))
    } else { Box::new(BufWriter::new(io::stdout())) };

    if let Some(input) = cmd.value_of("INPUT") {
      for line in BufReader::new(File::open(input)?).split(b'\n') {
        let line = line?;
        output.write(match predict(&info, &pinyin, &std::str::from_utf8(&line).unwrap_or(&gbk2string(&line)).split_whitespace().collect::<Vec<_>>()) {
          Ok(pinyin) => pinyin,
          Err(invalid) => format!(r#""{}" is not a valid pinyin"#, invalid),
        }.as_bytes())?;
        output.write(&[b'\n'])?;
      }
    } else {
      let mut rl = Editor::<()>::new();
      println!("Pinyin to Chinese Character REPL");
      println!("Enter [q] to exit; Arrow keys are supported");
      while let Ok(line) = rl.readline(">>> ") {
        if line.trim() == "q" { break; }
        if line.trim().is_empty() { continue; }
        rl.add_history_entry(line.as_ref());
        match predict(&info, &pinyin, &line.split_whitespace().collect::<Vec<_>>()) {
          Ok(pinyin) => println!("{}", pinyin),
          Err(invalid) => println!(r#""{}" is not a valid pinyin"#, invalid),
        }
      }
    }
    Ok(())
  }

  if let Err(err) = inner(&cmd) {
    println!("Error occurred! {:?}", err);
    println!("Hint: check whether these files/dir are present in current dir: input = {}, cache = {}, train = {}, pinyin = {}",
             cmd.value_of("INPUT").unwrap_or("stdin"),
             cmd.value_of("CACHE").unwrap_or("cache.bin"),
             cmd.value_of("TRAIN").unwrap_or("sina_news"),
             cmd.value_of("PINYIN").unwrap_or("拼音汉字表.txt"));
  }
}
