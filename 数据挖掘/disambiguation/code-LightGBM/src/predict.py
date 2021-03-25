import src.feature as feature
import src.train as train
from sklearn.cluster import AgglomerativeClustering
import json
import numpy as np
import lightgbm as lgm


def cluster_on_name(model, papers, batch_size):
    if len(papers) == 0:
        return []
    distance = np.zeros((len(papers), len(papers)))

    count = 0
    total = 0
    pairs = []
    predict = np.array([])
    for i in range(len(papers)):
        for j in range(i + 1, len(papers)):
            temp = [papers[i], papers[j]]
            np.random.shuffle(temp)
            pairs.append(temp)
            count += 1
            if count == batch_size or (i == len(papers) - 2 and j == len(papers) - 1):
                total += count
                print(f"finished {total} of {len(papers) * (len(papers) - 1) / 2}")
                f = np.array(feature.make_features(pairs, "valid", "", False))
                predict = np.concatenate([predict, model.predict(f)])
                count = 0
                pairs = []

    count = 0
    for i in range(len(papers)):
        for j in range(i + 1, len(papers)):
            distance[i][j] = 1 - predict[count]
            distance[j][i] = 1 - predict[count]
            count += 1

    distance = np.log(distance + 0.0001)

    clustering = AgglomerativeClustering(
        n_clusters=None,
        affinity="precomputed",
        linkage="average",
        compute_full_tree=True,
        distance_threshold=np.log(0.5),
    ).fit(distance)

    print(f"{clustering.n_clusters_} clusters are generated...")
    clusters = [[] for _ in range(clustering.n_clusters_)]
    labels = clustering.labels_
    for i in range(len(papers)):
        clusters[labels[i]].append(papers[i])

    return clusters


if __name__ == "__main__":
    model = lgm.Booster(model_file=train.save_path)
    au = json.load(open("./data/v2/valid/sna_valid_author_raw.json", "r"))
    for idx, (au_id, papers) in enumerate(au.items()):
        print(f"process same name author {idx} of {len(au)}")
        au[au_id] = cluster_on_name(model, papers, 500000)
    json.dump(au, open("./result.json", "w"), indent=4)
