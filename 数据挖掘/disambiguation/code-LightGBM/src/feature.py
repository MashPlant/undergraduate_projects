import src.pre as pre
import src.datasplit as datasplit
from multiprocessing import Pool
import re
import numpy as np
import pickle as pkl


# 计算两向量的余弦相似度，二者都是长度为300的np.array
def cosine_similarity(vec1, vec2):
    d1 = np.linalg.norm(vec1)
    d2 = np.linalg.norm(vec2)
    if d1 == 0 or d2 == 0:
        return 0  # 存在文本缺失
    return np.sum(vec1 * vec2) / (d1 * d2)


# org1/2都是字符串列表，计算两个机构的匹配度(pre.py中已经把它们都转成小写了)
def match_org(org1, org2):
    if len(org1) == 0 and len(org2) == 0:
        return -2  # 都缺失
    if len(org1) == 0 or len(org2) == 0:
        return -1  # 一个缺失
    if len(org1) > len(org2):
        org1, org2 = org2, org1
    org2 = set(org2)
    return sum((s in org2) for s in org1) / len(org1)  # bool值可以求和，每个当0/1算


# n1/2都是字符串列表
# 判断两个表示名字的字符串是否是同一个人，用的算法非常复杂，因为同一个名字可以有很多不同的英文写法
def match_name(n1, n2):
    if len(n1) == 0 or len(n2) == 0:
        return False  # 若有一为空，返回不匹配
    if len(n1) < len(n2):
        n1, n2 = n2, n1

    # 总长度，单字符个数
    len1, single1 = 0, 0
    len2, single2 = 0, 0
    for s in n1:
        len1 += len(s)
        single1 += len(s) == 1
    for s in n2:
        len2 += len(s)
        single2 += len(s) == 1

    # n2中只有一个串时与n1匹配策略
    if len(n2) == 1:
        n2 = n2[0]
        if single1 >= len(n1) - 1:
            return all(len(s) > 1 or s in n2 for s in n1)  # n1中的单字符是否全在n2中?
        else:
            return n2[0] in "".join(n1)

    # 若n1和n2长度相等，将整体短的存于n1
    if len(n1) == len(n2) and len1 > len2:
        n1, n2 = n2, n1
        single1, single2 = single2, single1

    # n1全为单字符时与n2的匹配策略
    if single1 == len(n1):
        n2 = "".join(n2)
        return re.match(".*".join(n1) + ".*", n2) is not None or \
               re.match(n1[-1] + ".*" + ".*".join(n1[:-1]) + ".*", n2) is not None

    # 尝试姓氏在第一位和最后一位
    flag1 = len(n1[0]) > 1 and n1[0] in n2
    flag2 = len(n1[-1]) > 1 and n1[-1] in n2
    if not flag1 and not flag2:  # 姓氏不匹配
        return False

    def judge(family_idx):
        family = n1[family_idx]
        if single1 == 0 and len1 != len2:
            return False
        return re.match(".*".join(s for s in n1 if s != family) + ".*",
                        "".join(s for s in n2 if s != family)) is not None

    return (flag1 and judge(0)) or (flag2 and judge(-1))


def match_authors(arg):
    authors1, authors2 = arg  # authors1/2类型都是 [{ "name": str, "org": str }]
    match_author_cnt = 0
    max_org_similarity = -2  # 机构相似度的最大值
    max_same_name_org_similarity = -2  # 名字相同的论文对中，机构相似度的最大值
    mean_org_similarity, cnt1 = 0, 0
    org_short_avg, cnt2 = 0, 0
    for au1 in authors1:
        org1, name1 = au1["org"], au1["name"]
        has_same_name = 0
        for au2 in authors2:
            org2, name2 = au2["org"], au2["name"]
            if len(org1) != 0 and len(org2) != 0:
                org_short_avg += min(len(org1), len(org2))
                cnt2 += 1
            org_similarity = match_org(org1, org2)
            max_org_similarity = max(max_org_similarity, org_similarity)
            if org_similarity >= 0:
                mean_org_similarity += org_similarity
                cnt1 += 1
            if match_name(name1, name2):
                has_same_name = 1
                max_same_name_org_similarity = max(max_same_name_org_similarity, org_similarity)
        match_author_cnt += has_same_name
    return [
        match_author_cnt,
        max_same_name_org_similarity,
        max_org_similarity,
        0 if cnt1 == 0 else mean_org_similarity / cnt1,
        0 if cnt2 == 0 else org_short_avg / cnt2,
    ]


meta_train = [pkl.load(open(f"./embedding_paper_meta_info/train-{m}.pkl", "rb"))
              for m in ("title", "keywords", "venue", "abstract")]
meta_valid = [pkl.load(open(f"./embedding_paper_meta_info/valid-{m}.pkl", "rb"))
              for m in ("title", "keywords", "venue", "abstract")]


# pairs: [(paper1 id, paper2 id, 这两篇paper是否属于同一簇(0/1)]
# pair_type:"train"或"valid"
# add_label: 是否把数据中的"这两篇paper是否属于同一簇"加到结果中
def make_features(pairs, pair_type, save_path, add_label=True):
    f = np.ndarray((len(pairs), 0), dtype='float32')  # 初始时每一列都是空的，后续往上面拼接

    pubs, meta = (pre.pub_train, meta_train) if pair_type == "train" else (pre.pub_test, meta_valid)

    # 年份小的放前面
    for pair in pairs:
        if pubs[pair[0]]["year"] > pubs[pair[1]]["year"]:
            pair[0], pair[1] = pair[1], pair[0]

    # 往f中加入other_feature相关的列
    print("make_features 1: appending other features into feature set...")
    temp = []
    for pair in pairs:
        paper1, paper2 = pubs[pair[0]], pubs[pair[1]]
        temp.append([
            *paper1["other_feature"], *paper2["other_feature"],
            *(paper1["other_feature"][i] * paper2["other_feature"][i] for i in range(6)),
            paper1["year"], paper2["year"], paper1["year"] - paper2["year"]
        ])
    f = np.c_[f, temp]
    # 往f中加入author相关的列
    print("make_features 2: matching the similarity feature of author's name and org...")
    # 这一步非常慢
    # 注意，虽然别的地方写的都不是很在意，比如open和Pool都没有用with，但是那些地方都只调用一次
    # 不用with的话都是有内存泄露的风险的(经测试open没有出现内存泄露，Pool出现了，当然这个其实是没有保证的)
    # 这里会调用多次，所以不用with内存就爆了
    # with Pool(4) as p:
    #     temp = p.map(match_authors, ((pubs[pair[0]]["authors"], pubs[pair[1]]["authors"]) for pair in pairs))
    # f = np.c_[f, temp]
    f = np.c_[f, [match_authors((pubs[pair[0]]["authors"], pubs[pair[1]]["authors"])) for pair in pairs]]
    # 往f中加入这四个meta相关的列
    print("make_features 3: appending title, keywords, venue, abstract features into feature set...")
    for meta1 in meta:
        for meta2 in meta:
            f = np.c_[f, [cosine_similarity(meta1[pair[0]], meta2[pair[1]]) for pair in pairs]]

    if add_label:
        f = np.c_[f, [pair[2] for pair in pairs]]

    if save_path:
        pkl.dump(np.array(f), open(save_path, "wb"))
    return f


if __name__ == "__main__":
    make_features(datasplit.train_pairs, "train", "./final_train_and_valid_set/train_training.pkl")
    make_features(datasplit.valid_pairs, "train", "./final_train_and_valid_set/train_validating.pkl")
