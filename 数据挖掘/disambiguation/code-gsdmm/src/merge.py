# 基于相似度进行合并
import json
import multiprocessing as mp
from collections import Counter
from src.util import *

similarity_threshold = 0.024


def workMerge(obj):
    # [0] = co_author, [1] = orgs, [2] = keywords_venue
    document = ([], [], [])

    # list of bool, True for this cluster is already used, and can't be merged
    used = []
    original_clusters = []

    word_count = (Counter(), Counter(), Counter())
    
    for cluster in obj:
        # won't merge cluster with size = 1
        used.append(len(cluster['papers']) == 1)
        for i, field in enumerate(('co_authors', 'orgs', 'keywords_venue')):
            field = Counter(cluster[field])
            word_count[i].update(field)
            document[i].append(field)

        original_clusters.append(cluster['papers'])

    # list of list of int, a element represent a set of merged clusters
    cluster_list = [[i] for i in range(len(original_clusters))]

    # fill document with (normalized) TF value, maybe we can use TF-IDF in the future
    for i in range(3):
        wc = word_count[i]
        word2id = {k: v for v, k in enumerate(wc.keys())}
        for j, d in enumerate(document[i]):
            vec = np.zeros(len(word2id), dtype='float32')
            for w, cnt in d.items():
                vec[word2id[w]] = cnt / wc[w]
            document[i][j] = normalized(vec)

    while True:
        max_sim = 0.0
        for i in range(len(used)):
            if used[i]: continue
            for j in range(i):
                if used[j]: continue
                # all vectors are normalized, so dot product == cosine distance
                sim1 = np.dot(document[0][i], document[0][j])
                sim2 = np.dot(document[1][i], document[1][j])
                sim3 = np.dot(document[2][i], document[2][j])
                sim = sim1 * 0.6 + sim2 * 0.2 + sim3 * 0.2
                if sim > max_sim:
                    max_sim, x, y = sim, i, j
        if max_sim >= similarity_threshold:  # assume x & y are initialized
            cluster_list[x].extend(cluster_list[y])
            cluster_list[y].clear()
            used[y] = True
            for d in document:
                d[x] = normalized(d[x] + d[y])
        else:
            break

    new_cluster = []
    for clusters in cluster_list:
        if len(clusters) == 0: continue
        new_cluster.append(concat_lists(original_clusters[i] for i in clusters))
    return new_cluster


def main():
    with open('data/authorList') as fp:
        authors = json.load(fp)
    with open('data/clusterResult') as fp:
        clusterResult = json.load(fp)
    
    new_clusters = mp.Pool(mp.cpu_count()).map(workMerge, clusterResult.values())
    result_dict = {author: cluster for author, cluster in zip(authors, new_clusters)}
    json.dump(result_dict, open('data/finalSubmit.json', 'w'), indent=4)


if __name__ == '__main__':
    main()
