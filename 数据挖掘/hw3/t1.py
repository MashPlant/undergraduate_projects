import os
import xml.etree.ElementTree as ET
import re
import string
import nltk
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from dataclasses import dataclass
from typing import Tuple, Set, List
from collections import Counter
from nltk.corpus import stopwords
from nltk.stem import WordNetLemmatizer 
from wordcloud import WordCloud

DIR = 'samples_500/'

@dataclass
class Doc:
  text: str
  words: Counter
  time: Tuple[int, int, int]
  classifier: Set[str]

def preprocess(text: str) -> Counter:
  text = text.lower().translate(str.maketrans('', '', string.punctuation + string.digits))
  tokens = nltk.word_tokenize(text)
  s = stopwords.words('english')
  w = WordNetLemmatizer()
  return Counter("say" if x == "said" else w.lemmatize(x) for x in tokens if x not in s)

def show_wordcloud(words: Counter):
  top = { k: v for k, v in words.most_common(100) }
  wc = WordCloud(background_color = "white").generate_from_frequencies(top)
  plt.figure()
  plt.imshow(wc)
  plt.axis("off")
  plt.savefig('wordcloud.png', bbox_inches = 'tight')

def wordlen_hist(words: Counter):
  plt.figure()
  plt.hist([len(x) for x in words.keys()])
  plt.savefig('wordlen_hist0.png', bbox_inches = 'tight')
  plt.figure()
  plt.hist(sum(([len(x)] * y for x, y in words.items()), start = []))
  plt.savefig('wordlen_hist1.png', bbox_inches = 'tight')

def wordcnt_hist(docs: List[Doc]):
  wordcnt = list(sum(x.words.values()) for x in docs if x.words is not None)
  for i, method in enumerate((pd.cut, pd.qcut)):
    plt.figure()
    newscnt, bound = method(wordcnt, 10, retbins = True)
    newscnt = newscnt.value_counts()
    plt.bar((bound[1:] + bound[:-1]) / 2, newscnt, width = (bound[1:] - bound[:-1]) * 0.9)
    for j, x in enumerate(newscnt):
      plt.text((bound[j] + bound[j + 1]) / 2, x + 3, str(x), horizontalalignment = 'center')
    plt.savefig(f'wordcnt_hist{i}.png', bbox_inches = 'tight')

def classifier_hist(docs: List[Doc]):
  classifier = Counter()
  for x in docs:
    for c in x.classifier:
      classifier[c] += 1
  classifier = classifier.most_common()
  plt.figure()
  ax = plt.subplot(111)
  pos = ax.get_position()
  ax.set_position([pos.x0, pos.y0 + pos.height * 0.3, pos.width, pos.height * 0.8])
  xs = np.arange(len(classifier)) + 1
  plt.bar(xs, [x[1] for x in classifier])
  plt.xticks(xs, [x[0] for x in classifier], rotation = 90)
  plt.savefig('classifier_hist.png')

def month_hist(docs: List[Doc]):
  plt.figure()
  xs = np.arange(12) + 1
  plt.bar(xs, [sum((1 for x in docs if x.time[1] == i)) for i in xs]) # trash language
  plt.xticks(xs, xs)
  plt.savefig('month_hist.png')

docs = []
all_words = Counter()
for f in os.listdir(DIR):
  t = ET.ElementTree(file=DIR + f)
  classifier_re = re.compile(r'Top/(News|Features)/(.*?)/')
  text, words, y, m, d, classifier = None, None, None, None, None, set()
  if (x := t.find('.//block[@class="full_text"]')) is not None:
    text = str.join('\n', map(lambda x: x.text, x.iter()))
    words = preprocess(text)
    all_words.update(words)
  if (x := t.find('.//meta[@name="publication_year"]')) is not None:
    y = int(x.attrib['content'])
  if (x := t.find('.//meta[@name="publication_month"]')) is not None:
    m = int(x.attrib['content'])
  if (x := t.find('.//meta[@name="publication_day_of_month"]')) is not None:
    d = int(x.attrib['content'])
  for x in t.iterfind('.//classifier'):
    for c in classifier_re.findall(x.text):
      classifier.add(c[1])
  docs.append(Doc(text, words, (y, m, d), classifier))
show_wordcloud(all_words)
wordlen_hist(all_words)
wordcnt_hist(docs)
classifier_hist(docs)
month_hist(docs)