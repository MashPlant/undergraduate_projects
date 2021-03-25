use std::{collections::HashMap, fs};
use noisy_float::prelude::*;

struct Doc<'a> {
  // file name (parsed into u32)
  id: u32,
  // all words count in this file
  cnt: u32,
  // tf-idf vector
  words: HashMap<&'a str, f64>,
}

fn main() {
  let mut buf = Vec::new();
  for d in fs::read_dir("nyt_corp0").unwrap() {
    let path = d.unwrap().path();
    let id = path.file_name().unwrap().to_string_lossy().parse().unwrap();
    let mut text = fs::read_to_string(&path).unwrap();
    // punctuations are ignored, but spaces are retained to separate words
    text.retain(|ch| ch.is_ascii_alphanumeric() || ch.is_ascii_whitespace());
    buf.push((id, text));
  }
  // map word to how many docs does this word exists in
  let mut all = HashMap::<_, u32>::new();
  // C[i, j] and C[j, i] will be both counted for co-occurrence of i & j
  let mut co_occurrence = HashMap::<_, HashMap<_, _>>::new();
  let mut docs = buf.iter().map(|(id, text)| {
    let mut cnt = 0;
    let mut words = HashMap::new();
    for w in text.split_ascii_whitespace() {
      cnt += 1;
      *words.entry(w).or_default() += 1.0;
    }
    for (&w, _) in &words {
      *all.entry(w).or_default() += 1;
      for (&w1, _) in &words {
        *co_occurrence.entry(w).or_default().entry(w1).or_default() += 1;
      }
    }
    Doc { id: *id, cnt, words }
  }).collect::<Vec<_>>();
  let n = docs.len() as f64;
  for doc in &mut docs {
    // old "tf_idf" value is word count
    for (&w, tf_idf) in &mut doc.words {
      *tf_idf = *tf_idf / doc.cnt as f64 * (n / *all.get(w).unwrap() as f64).ln();
    }
  }
  // just two arbitrarily chosen values
  doc_similarity(&mut docs, 52);
  word_similarity(&co_occurrence, "crime");
}

fn doc_similarity(docs: &mut [Doc], id: u32) {
  println!("doc similarity with doc {}", id);
  let target = docs.iter().find(|d| d.id == id).unwrap().words.clone();
  docs.sort_by_cached_key(|Doc { words, .. }| {
    r64(words.iter().map(|(&w, &t)| {
      (t - target.get(w).copied().unwrap_or_default()).powi(2)
    }).sum::<f64>() + target.iter().map(|(&w, &t)| {
      if !words.contains_key(w) { t.powi(2) } else { 0.0 }
    }).sum::<f64>())
  });
  println!("by euclidean distance: {:?}", docs[1..6].iter().map(|d| d.id).collect::<Vec<_>>());
  docs.sort_by_cached_key(|Doc { words, .. }| {
    let dot = words.iter().map(|(&w, &t)| {
      t * target.get(w).copied().unwrap_or_default()
    }).sum::<f64>();
    let base = (words.values().map(|&t| t.powi(2)).sum::<f64>() *
      target.values().map(|&t| t.powi(2)).sum::<f64>()).sqrt();
    r64(-dot / base)
  });
  println!("by cosine distance: {:?}", docs[1..6].iter().map(|d| d.id).collect::<Vec<_>>());
}

fn word_similarity(co_occurrence: &HashMap<&str, HashMap<&str, u32>>, word: &str) {
  println!("word similarity with word {:?}", word);
  let target = co_occurrence.get(word).unwrap();
  let mut co_occurrence = co_occurrence.iter().map(|(&s, map)|
    (s, map)).collect::<Vec<_>>();
  co_occurrence.sort_by_cached_key(|&(_, map)| {
    map.iter().map(|(&w, &n)| {
      (n as i64 - target.get(w).copied().unwrap_or_default() as i64).pow(2) as u64
    }).sum::<u64>() + target.iter().map(|(&w, &n)| {
      if !map.contains_key(w) { (n as u64).pow(2) } else { 0 }
    }).sum::<u64>()
  });
  println!("by euclidean distance: {:?}", co_occurrence[1..6].iter().map(|&(s, _)| s).collect::<Vec<_>>());
  co_occurrence.sort_by_cached_key(|&(_, map)| {
    let dot = map.iter().map(|(&w, &n)| {
      n as u64 * target.get(w).copied().unwrap_or_default() as u64
    }).sum::<u64>() as f64;
    let base = ((map.values().map(|&x| (x as u64).pow(2)).sum::<u64>() *
      target.values().map(|&x| (x as u64).pow(2)).sum::<u64>()) as f64).sqrt();
    r64(-dot / base)
  });
  println!("by cosine distance: {:?}", co_occurrence[1..6].iter().map(|&(s, _)| s).collect::<Vec<_>>());
}