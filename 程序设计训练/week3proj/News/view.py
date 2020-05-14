from django.shortcuts import render
from django.http import HttpRequest
import datetime
import time
import os
import pickle
from typing import List, Dict
from collections import Counter
import jieba
from gensim import corpora, models, similarities
from . import query_parser


class News:
    def __init__(self, file_name):
        self.title = file_name
        self.index = -1  # late init
        self.keywords = []  # late init
        self.similar = []  # late init
        with open(file_name, 'r', encoding='UTF-8')as f:
            lines = f.read().split('\n')
            self.time = lines[0].split()[0]
            self.parsed_time = datetime.datetime.strptime(
                self.time, '%Y年%m月%d日%H:%M')
            self.content = lines[1:]

    @property
    def href(self):
        return '/?article=' + str(self.index)

    @property
    def keywords_with_href(self):
        return [{'word': word, 'href': '/?search=' + word} for word in self.keywords]

    @property
    def similar_with_href(self):
        return [{'title': news_pieces[word].title, 'href': '/?article=' + str(word)} for word
                in self.similar]

    @property
    def unmarked_title(self):
        return self.title


class NewsWithSearch:
    def __init__(self, news: News, search_words: List[str]):
        self.news = news
        self.search_words = search_words

    def get_marked_str(self, s):
        for word in self.search_words:
            s = s.replace(word, '<mark>' + word + '</mark>')
        return s

    # overrides
    @property
    def href(self):
        return '/?article=' + str(self.news.index) + '&search=' + '+'.join(self.search_words)

    @property
    def content(self):
        return (self.get_marked_str(s) for s in self.news.content)

    @property
    def title(self):
        return self.get_marked_str(self.news.title)

    # inherit
    @property
    def time(self):
        return self.news.time

    @property
    def keywords_with_href(self):
        return self.news.keywords_with_href

    @property
    def similar_with_href(self):
        return self.news.similar_with_href

    @property
    def unmarked_title(self):
        return self.news.title


stop_words = {'的', '了', '在', '是', '我', '有', '和', '就',
              '不', '人', '都', '一', '一个', '上', '也', '很', '到', '说', '要', '去', '你',
              '会', '着', '没有', '看', '好', '自己', '这', '他', '她', '它',
              '，', ',', '。', '.', '；', ';', '（', '(', '）', ')', '“', '"', '”', '！', '!',
              '、', '：', ':', '？', '?', '《', '》', '【', '[', '】', ']'}

news_pieces: List[News] = []
invert_dict: Dict[str, Counter] = {}


def prepare():
    os.chdir('News/data')
    jieba.enable_parallel()  # avoid future wait

    def walk(target):
        for file in os.listdir(target):
            path = os.path.join(target, file)
            if not os.path.isdir(path):
                yield file

    global invert_dict
    global news_pieces

    try:
        news_pieces, invert_dict = pickle.load(open('data', 'rb'))
    except:
        for name in walk('.'):
            if not name.endswith('data'):
                news_pieces.append(News(name))
        news_pieces.sort(key=lambda x: x.parsed_time, reverse=True)
        for i, news in enumerate(news_pieces):
            news.index = i
        jieba.enable_parallel()
        print('building cut list')
        cut_list = [list(filter(lambda x: x not in stop_words and not x.isspace(),
                                jieba.cut_for_search(''.join(news.content)))) for news in news_pieces]
        print('built cut list')
        print('building invert dict')
        for i, cut in enumerate(cut_list):
            doc_dict = Counter()  # local counter
            for word in cut:
                if word not in stop_words and not word.isspace():
                    doc_dict[word] += 1
            news_pieces[i].keywords = [x[0] for x in doc_dict.most_common(5)]
            for word, cnt in doc_dict.items():
                if word in invert_dict:
                    invert_dict[word][i] = cnt
                else:
                    invert_dict[word] = Counter({i: cnt})
        print('built invert dict')
        print('builing similar dict')
        dictionary = corpora.Dictionary(cut_list)
        print('built dictionary')
        corpus = [dictionary.doc2bow(cut) for cut in cut_list]
        print('built corpus')
        tfidf = models.TfidfModel(corpus)
        print('built model')
        index = similarities.SparseMatrixSimilarity(
            tfidf[corpus], num_features=len(dictionary.keys()))
        print('built index')
        N = len(news_pieces)
        for i in range(len(news_pieces)):
            similar = sorted(
                enumerate(index[tfidf[corpus[i]]]), key=lambda x: x[1], reverse=True)[1:6]
            for x in similar:
                news_pieces[i].similar.append(x[0])
            s = int(i / N * 20)
            print(f'{i}/{N}', end=' ')
            for _ in range(s):
                print('▓', end='')
            for _ in range(s, 20):
                print('░', end='')
            print('\r', end='')
        pickle.dump((news_pieces, invert_dict), open('data', 'wb'))


prepare()


def gen_fn(x):
    try:
        return datetime.datetime.strptime(x, '%Y-%m-%d'), None
    except:
        try:
            return datetime.datetime.strptime(x, '%y-%m-%d'), None
        except:
            res = Counter()
            words = []
            for w in jieba.cut(x):
                res += Counter(invert_dict[w]) if w in invert_dict else Counter()
                words.append(w)
            return res, words


def before_fn(x, y):
    for k in list(x.keys()):
        if news_pieces[k].parsed_time > y:
            del x[k]


def after_fn(x, y):
    for k in list(x.keys()):
        if news_pieces[k].parsed_time < y:
            del x[k]


max_news_per_page = 100
max_page_per_page = 15
half_page_per_page = 7


def get(request: HttpRequest):
    if 'article' in request.GET:
        return get_article(request)
    return get_page(request)


def get_page(request: HttpRequest):
    beg = time.time()
    try:
        page = int(request.GET['page'])
    except:
        page = 0
    try:
        search = request.GET['search'].strip()
    except:
        search = ''
    search_escape = search.replace(' ', '&nbsp;')
    if search:
        try:
            result, search_words = query_parser.get_result(search, gen_fn=gen_fn, before_fn=before_fn,
                                                           after_fn=after_fn)
            result = [NewsWithSearch(news_pieces[i[0]], search_words) for i in result.most_common()]
        except:
            return render(request, 'pages.html', {'error': 'Invalid query'})
    else:
        result = news_pieces
    max_page = len(result) // max_news_per_page
    if max_page == 0:
        max_page = 1
    if page >= max_page or page < 0:
        return render(request, 'pages.html', {'error': 'Index out of range'})
    if max_page > max_page_per_page:
        lb, ub = page - half_page_per_page, page + half_page_per_page
        forward_more, backward_more = True, True
        if lb <= 0:
            forward_more = False
            lb, ub = 0, max_page_per_page
        if ub > max_page:
            backward_more = False
            lb, ub = max_page - max_page_per_page, max_page
    else:
        lb, ub = 0, max_page
        forward_more, backward_more = False, False
    base_href = f'/?search={search.replace(" ","+")}&page=' if search else '/?page='
    return render(request, 'pages.html',
                  {'page_list': [{'num': i, 'href': base_href + str(i)} for i in range(lb, ub)],
                   'data_list': result[max_news_per_page * page:
                                       len(result) if page == max_page - 1 else max_news_per_page * (page + 1)],
                   'cur_page': page,
                   'forward_more': forward_more, 'backward_more': backward_more,
                   'search_info': f'找到新闻{len(result)}项，用时{time.time()-beg}秒',
                   'input_search': search_escape,
                   })


def get_article(request):
    article_num = int(request.GET['article'])
    article = news_pieces[article_num]
    if 'search' in request.GET:
        article = NewsWithSearch(article, request.GET['search'].split(' '))
    return render(request, 'article.html', {'news_num': article_num, 'news': article})

# def get_page(request, page_num):
#     try:
#         page_num = int(page_num)
#     except:
#         raise Http404(f'invalid page number {page_num}')
#     if page_num < 0 or page_num >= max_page:
#         raise Http404(f'invalid page number {page_num}')
#
#     lb, ub = page_num - half_page_per_page, page_num + half_page_per_page
#     forward, backward_more = True, True
#     if lb <= 0:
#         forward = False
#         lb, ub = 0, page_per_page
#     if ub > max_page:
#         backward_more = False
#         lb, ub = max_page - page_per_page, max_page
#
#     return render(request, 'pages.html',
#                   {'page_list': [{'num': i, 'href': '/page/' + str(i)} for i in range(lb, ub)],
#                    'data_list': news_pieces[news_per_page * page_num:
#                                             len(news_pieces) if page_num == max_page - 1 else news_per_page * (
#                                                     page_num + 1)],
#                    'cur_page': page_num,
#                    'forward_more': forward, 'backward_more': backward_more,
#                    'test_mark': '<mark>hello</mark>'})
#
#
# def goto_page(request):
#     return get_page(request, request.GET['page'])
#
#
# def get_search(request):
#     keyword = request.GET['keyword']
#     keyword_escape = keyword.replace(' ', '&nbsp;')
#
#     def gen_fn(x):
#         try:
#             return datetime.datetime.strptime(x, '%Y-%m-%d')
#         except:
#             try:
#                 return datetime.datetime.strptime('20' + x, '%Y-%m-%d')
#             except:
#                 return Counter(invert_dict[x]) if x in invert_dict else Counter()
#
#     def before_fn(x, y):
#         for k in list(x.keys()):
#             if news_pieces[k].parsed_time > y:
#                 del x[k]
#
#     def after_fn(x, y):
#         for k in list(x.keys()):
#             if news_pieces[k].parsed_time < y:
#                 del x[k]
#
#     try:
#         beg = time.time()
#         result = query_parser.get_result(
#             keyword, gen_fn=gen_fn, before_fn=before_fn, after_fn=after_fn)
#         return render(request, 'pages.html', {'data_list': [news_pieces[i[0]] for i in result.most_common()],
#                                               'default_value': keyword_escape,
#                                               'search_info': f'找到新闻{len(result)}项，用时{time.time()-beg}秒'})
#     except:
#         return render(request, 'pages.html', {'default_value': keyword_escape, 'error': True})
#
#
# def get_article(request, news_num):
#     news_num = int(news_num)
#     news = news_pieces[news_num]
#     return render(request, 'article.html', {'news_num': news_num, 'news': news})
