import math
import pickle as pkl
import numpy as np
import src.pre as pre

train_path = "./train_and_valid_pairs/train_pairs.pkl"
valid_path = "./train_and_valid_pairs/valid_pairs.pkl"

try:
    train_pairs = pkl.load(open(train_path, "rb"))
    valid_pairs = pkl.load(open(valid_path, "rb"))
except:
    print("这是第一次运行datasplit.py，清洗过的数据尚未生成")  # 与pre.py同理


# 在candidates[start:]中随机抽出sample_rate比例的不重复数与fst组成训练对，pos = 1表示训练对为正例，否则为负例
def random_sample(candidates, start, fst, rate, pos):
    size = len(candidates) - start
    if size <= 0:
        return []
    sample_num = max(math.floor(size * rate), 1)
    indices = np.arange(size)
    np.random.shuffle(indices)
    return [[fst, candidates[start + idx], pos] for idx in indices[:sample_num]]


# 利用聚类的同名作者文件采样生成[paper1, paper2, label]形式的多个训练或验证pair
# result: 聚类结果，即 { 作者名 -> { 作者id -> 文章id列表 } } 的二重dict
# pos_rate / neg_rate分别为正 / 负样本采样率
def create_pairs(result, pos_rate, neg_rate):
    pairs = []
    for name, clusters in result.items():
        papers = [paper for cluster in clusters.values() for paper in cluster]
        count = 0
        for c in clusters.values():
            count += len(c)
            for i in range(len(c)):
                pairs.extend(random_sample(c, i + 1, c[i], pos_rate, 1))
                pairs.extend(random_sample(papers, count, c[i], neg_rate, 0))
    np.random.shuffle(pairs)
    return pairs


if __name__ == "__main__":
    o_t_au = pre.train_author
    au_names = list(o_t_au.keys())
    np.random.shuffle(au_names)

    t_size = int(len(au_names) * 0.8)

    t = {name: o_t_au[name] for name in au_names[:t_size]}
    v = {name: o_t_au[name] for name in au_names[t_size:]}
    pkl.dump(create_pairs(t, 0.075, 0.0076), open(train_path, "wb"))
    pkl.dump(create_pairs(v, 0.0925, 0.007), open(valid_path, "wb"))
