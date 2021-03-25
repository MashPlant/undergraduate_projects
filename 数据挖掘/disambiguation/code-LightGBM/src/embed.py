import gensim.downloader as api
import numpy as np
import pickle as pkl
import src.pre as pre

w2v_model = api.load("glove-wiki-gigaword-300").wv


def embed(string):
    ret = np.zeros(300)
    cnt = 0
    for word in string.split():
        if word in w2v_model:
            ret += w2v_model[word]
            cnt += 1
        elif word.lower() in w2v_model:
            ret += w2v_model[word.lower()]
            cnt += 1
    if cnt != 0:
        ret /= cnt
    return ret


# 对title & keywords & venue & abstract四种信息进行embedding
def embed_pubs(pubs, prefix):
    for meta in ("title", "keywords", "venue", "abstract"):
        result = {paper_id: embed(paper[meta]) for paper_id, paper in pubs.items()}
        pkl.dump(result, open(f"./embedding_paper_meta_info/{prefix}-{meta}.pkl", "wb"))


if __name__ == "__main__":
    embed_pubs(pre.pub_train, "train")
    embed_pubs(pre.pub_test, "valid")
