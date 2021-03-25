# 计算验证集上的评测结果
import json
from collections import defaultdict


def main():
    pred_dict = {}
    # map (truth_cluster, pred_cluster) to # papers in the truth cluster
    correct_dict = defaultdict(int)
    correct_pairs = 0
    pred_pairs = 0
    truth_pairs = 0

    pred_cluster_id = 0
    for author, clusters in json.load(open('finalSubmit.json')).items():
        for cluster in clusters:
            pred_cluster_id += 1
            pred_pairs += len(cluster) * (len(cluster) - 1) // 2
            for paper in cluster:
                pred_dict[(author, paper)] = pred_cluster_id

    truth_cluster_id = 0
    for author, clusters in json.load(open('data/sna_data/sna_valid_author_ground_truth.json')).items():
        for cluster in clusters.values():
            truth_cluster_id += 1
            truth_pairs += len(cluster) * (len(cluster) - 1) // 2
            for paper in cluster:
                pred_cluster = pred_dict[(author, paper)]
                truth_cluster = truth_cluster_id
                correct_pairs += correct_dict[(truth_cluster, pred_cluster)]
                correct_dict[(truth_cluster, pred_cluster)] += 1

    precision = correct_pairs / pred_pairs
    recall = correct_pairs / truth_pairs
    f1 = 2 * precision * recall / (precision + recall)
    print('pairwise precision: ', round(precision, 6))
    print('pairwise recall: ', round(recall, 6))
    print('pairwise f1: ', round(f1, 6))


if __name__ == '__main__':
    main()
