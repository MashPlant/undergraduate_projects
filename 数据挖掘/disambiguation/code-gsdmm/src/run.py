import multiprocessing as mp
import json

from src.data_process import generate_paper_set_and_authors
from src.gsdmm import workGsdmm
from src.make_cluster import makeCluster
from src.merge import workMerge


def main():
    authorInfo, authorList = generate_paper_set_and_authors()
    gsdmmResult = mp.Pool(mp.cpu_count()).map(workGsdmm, authorInfo.values())
    clusterResult = makeCluster(authorList, authorInfo, gsdmmResult)
    new_clusters = mp.Pool(mp.cpu_count()).map(workMerge, clusterResult.values())
    result_dict = {author: cluster for author, cluster in zip(authorList, new_clusters)}
    json.dump(result_dict, open('data/finalSubmit.json', 'w'), indent=4)


if __name__ == "__main__":
    main()
