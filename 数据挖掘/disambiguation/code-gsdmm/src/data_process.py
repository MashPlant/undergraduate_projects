# 将数据按作者保存为一次处理文件，并进行预处理。
import json
import re
from nltk.stem.wordnet import WordNetLemmatizer
from nltk.corpus import stopwords
from multiprocessing import Pool

unicode_translate = str.maketrans(
    '\u00e0\u00e1\u00e2\u00e3\u00e4\u00e5\u00e6\u00e7\u00e8\u00e9\u00ea\u00eb\u00ec\u00ed\u00ef\u00f1\u00f2\u00f3\u00f4\u00f6\u00f8\u00f9\u00fa\u00fc\u00fd\u0103\u0107\u010d\u011b',
    'aaaaaaaceeeeiiinooooouuuyacce')

org_stopwords = set(stopwords.words('english')) | {'university', 'institute', 'department'}
title_stopwords = set(stopwords.words('english')) | {'study', 'based', 'effect', 'analysis', 'system', 'method', 'using', 'model', 'synthesis', 'application'}
abstract_stopwords = set(stopwords.words('english')) | {'result', 'method', 'using', 'based', 'high', 'used', 'different', 'show', 'also', 'two', 'proposed', 'structure', 'al', 'et', 'performance',
                                                        'new', 'respectively', 'compared', 'higher', 'one', 'could', 'however', 'may', 'found', 'obtained', 'significant', 'case', 'three', 'well',
                                                        'important', 'design', 'type', 'conclusion'}
venue_stopwords = set(stopwords.words('english')) | {'journal', 'university', 'conference'}


def process_name(s):
    s = s.lower()
    s = s.replace('_', ' ')
    s = s.replace('.', ' ')
    s = s.replace('-', '')
    s = s.translate(unicode_translate)
    s = s.split()
    s.sort()
    return '_'.join(s)


def rm_stopwords(stopwords, s):
    l = WordNetLemmatizer()
    return [l.lemmatize(w) for w in s.split() if w not in stopwords and len(w) > 1]


# 去标点
def etl(s):
    return re.sub(r"[\s+.!/,;|:${&~}<=>?%^\[\]*(\"\')]+|[+—()?【】“”！，。？、~@#￥%…&*（）]+", " ", s)


def process_org(s):
    s = etl(s).lower()
    s = s.replace('Sch.', 'school')
    s = s.replace('Dept.', 'department')
    s = s.replace('Coll.', 'college')
    s = s.replace('Inst.', 'institute')
    s = s.replace('Univ.', 'university')
    s = s.replace('Lab', 'laboratory')
    s = s.replace('Lab.', 'laboratory')
    s = s.replace('Natl.', 'national')
    s = s.replace('Comp.', 'computer')
    s = s.replace('Sci.', 'science')
    s = s.replace('Tech.', 'technology')
    s = s.replace('Technol.', 'technology')
    s = s.replace('Elec.', 'electronic')
    s = s.replace('Engr.', 'engineering')
    s = s.replace('Aca.', 'academy')
    s = s.replace('Syst.', 'systems')
    s = s.replace('Eng.', 'engineering')
    s = s.replace('Res.', 'research')
    s = s.replace('Appl.', 'applied')
    s = s.replace('Chem.', 'chemistry')
    s = s.replace('Prep.', 'petrochemical')
    s = s.replace('Phys.', 'physics')
    s = s.replace('Mech.', 'mechanics')
    s = s.replace('Mat.', 'material')
    s = s.replace('Cent.', 'center')
    s = s.replace('Ctr.', 'center')
    s = s.replace('Behav.', 'behavior')
    s = s.replace('Atom.', 'atomic')
    return rm_stopwords(org_stopwords, s)


def work(arg):
    author_id, author_name, papers = arg
    # author_name will not be considered as co_author
    author_name = process_name(author_name)
    result = {}
    for paper in papers:
        co_authors = []
        for name_org in paper['authors']:
            name = process_name(name_org['name'])
            if name != author_name:
                co_authors.append(name)
            co_authors.extend(process_org(name_org.get('org', '')))
        keywords = paper.get('keywords')
        if not keywords:  # it may be None or ""
            keywords = []
        result[paper['id']] = {
            'co_authors': co_authors,
            'title_abstract': rm_stopwords(title_stopwords, etl(paper['title']).lower()) +
                              rm_stopwords(abstract_stopwords, etl(paper.get('abstract', '')).lower()),
            'keywords_venue': [x for k in keywords for x in etl(k).lower().split()] +
                              rm_stopwords(venue_stopwords, etl(paper.get('venue', '')).lower())
        }
    json.dump(result, open(f'data/{author_id}', 'w'))
    return result


def generate_paper_set_and_authors():
    paper_data = json.load(open('data/sna_data/sna_valid_pub.json'))
    author_data = json.load(open('data/sna_data/sna_valid_author_raw.json'))
    authors = []
    for author, papers in author_data.items():
        author_data[author] = [paper_data[p] for p in papers]
        authors.append(author)
    json.dump(authors, open('data/authors', 'w'))
    results = Pool(16).map(work, ((i, author_name, author_data[author_name]) for i, author_name in enumerate(authors)))
    return results, authors


if __name__ == '__main__':
    generate_paper_set_and_authors()
