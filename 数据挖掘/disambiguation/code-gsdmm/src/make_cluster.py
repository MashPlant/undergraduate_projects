# 将结果簇聚集在一起
import itertools
import json
from src.util import *
from collections import Counter


def makeCluster(authors, authorInfo, gsdmmResult):
    res = {}
    for authorNo in range(len(authors)):
        if authors[authorNo] == 'NoPaper':
            continue

        cluster_result = gsdmmResult[authorNo]

        for doc in authorInfo:
            doc['cluster'] = cluster_result[doc['paperID']]
        authorInfo.sort(key=lambda x: x['cluster'])

        output_docs = []
        for _, cluster in itertools.groupby(authorInfo, key=lambda x: x['cluster']):
            cluster = list(cluster)
            size = len(cluster)
            ids = concat_lists(x['paperID'].split() for x in cluster)
            orgs = list(set(y for x in cluster for y in x['orgs'].split()))
            keywords_venue = concat_lists(x['keywords'].split() for x in cluster) + concat_lists(x['venue'].split() for x in cluster)
            co_authors = concat_lists(x['co-authors'].split() for x in cluster)
            if size > 3:
                co_authors = list(x[0] for x in Counter(co_authors).items() if x[1] > 1)
                keywords_venue = list(x[0] for x in Counter(keywords_venue).items() if x[1] > 2)
            else:
                co_authors = list(set(co_authors))
                keywords_venue = list(set(keywords_venue))
            obj = {'papers': ids, 'co_authors': co_authors, 'keywords_venue': keywords_venue, 'orgs': orgs}
            output_docs.append(obj)
        res[authorNo] = output_docs
    return res


def main():
    with open('data/authorList') as fp:
        authors = json.load(fp)
    with open('data/gsdmmResult') as fp:
        gsdmmResult = json.load(fp)
    with open('data/authorInfo') as fp:
        authorInfo = json.load(fp)
    res = makeCluster(authors, authorInfo, gsdmmResult)
    with open('data/clusterResult', 'w') as fp:
        json.dump(res, fp)


if __name__ == '__main__':
    main()
