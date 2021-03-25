import numpy as np
import json
import multiprocessing as mp

alpha = (0.09, 0.08)
beta = (0.11, 0.08)
K = 65  # initial cluster count
IT = 5


# all_docs: tuple of list of list of strings, each of its elements (list of list of strings) is a kind of doc
# currently we assume len(all_docs) == 2
# each of it elements has the same size (D)
def gsdmm(all_docs):
    D = len(all_docs[0])
    all_vocab_size, all_doc_arrays, all_cluster_sizes, all_cluster_wcs, all_cluster_words = [], [], [], [], []

    for docs in all_docs:
        word2id = {}
        for doc in docs:
            for w in doc:
                if w not in word2id:
                    word2id[w] = len(word2id)
        vocab_size = len(word2id)
        doc_arrays = np.zeros((D, vocab_size))
        for i, d in enumerate(docs):
            doc_array = doc_arrays[i]
            for w in d:
                doc_array[word2id[w]] += 1
        cluster_sizes = np.zeros(K)  # how many docs there are in this cluster
        cluster_wcs = np.zeros(K)  # wcs = word counts, cluster_wcs[i] == np.sum(cluster_words[i])
        cluster_words = np.zeros((K, vocab_size))  # cluster_words[i][j] == how many times the j word appears in cluster cluster i

        all_vocab_size.append(vocab_size), all_doc_arrays.append(doc_arrays), all_cluster_sizes.append(cluster_sizes), all_cluster_wcs.append(cluster_wcs), all_cluster_words.append(cluster_words)

    labels = np.random.choice(K, D)  # what cluster does this doc belong to, initially uniform random select D numbers from 0..K
    for i, l in enumerate(labels):
        for j in range(2):
            doc = all_doc_arrays[j][i]
            all_cluster_sizes[j][l] += 1
            all_cluster_wcs[j][l] += sum(doc)
            all_cluster_words[j][l] += doc

    def score(doc, j):
        ld1 = np.log((D - 1) + K * alpha[j])
        arange = np.arange(int(np.sum(doc))) + all_vocab_size[j] * beta[j]
        cluster_sizes, cluster_wcs, cluster_words = all_cluster_sizes[j], all_cluster_wcs[j], all_cluster_words[j]
        ret = np.zeros(K)
        for i in range(K):
            ln1 = np.log(cluster_sizes[i] + alpha[j])
            ln2 = np.dot(doc, np.log(cluster_words[i] + beta[j]))
            ld2 = np.sum(np.log(arange + cluster_wcs[i]))
            ret[i] = ln1 - ld1 + ln2 - ld2
        min, argmax = np.min(ret), np.argmax(ret)
        if ret[argmax] - min > 100:  # e^100 ~ 10^43
            ret.fill(0)
            ret[argmax] = 1
            return ret
        else:
            ret = np.exp(ret - min)
            return ret / np.sum(ret)

    for it in range(IT):
        changed = False
        for i in range(D):
            old_l = labels[i]
            for j in range(2):
                doc = all_doc_arrays[j][i]
                all_cluster_sizes[j][old_l] -= 1
                all_cluster_wcs[j][old_l] -= sum(doc)
                all_cluster_words[j][old_l] -= doc
            # final probability is weighted result
            p = 0.8 * score(all_doc_arrays[0][i], 0) + 0.2 * score(all_doc_arrays[1][i], 1)
            new_l = np.random.choice(K, 1, p=p)
            labels[i] = new_l
            for j in range(2):
                doc = all_doc_arrays[j][i]
                all_cluster_sizes[j][new_l] += 1
                all_cluster_wcs[j][new_l] += sum(doc)
                all_cluster_words[j][new_l] += doc
            if old_l != new_l:
                changed = True
        if not changed:
            break
    return labels


def workGsdmm(paperList):
    np.random.seed(19260817)
    ids = []
    co_authors = []
    titles = []
    keywords = []
    for paper in paperList:
        ids.append(paper['paperID'])
        co_authors.append(paper['co-authors'].split() + paper['orgs'].split())
        titles.append(paper['title'].split() + paper['abstract'].split())
        keywords.append(paper['keywords'].split())

    result = gsdmm((co_authors, titles, keywords))
    ret = {}
    for i, id in enumerate(ids):
        ret[id] = int(result[i])
    return ret

def main():
    with open('data/authorInfo') as fp:
        data = json.load(fp)
    ret = mp.Pool(mp.cpu_count()).map(workGsdmm, data.values())
    result = dict(zip(data.keys(), ret))
    with open('data/gsdmmResult', 'w') as fp:
        json.dump(result, fp)


if __name__ == '__main__':
    main()
