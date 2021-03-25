import requests as req
import os
import json

header = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36',
    'Referer': 'https://www.aminer.cn/whoiswho',
    'Host': 'lfs.aminer.cn',
    'Connection': 'close',
    'Upgrade-Insecure-Requests': '1',
}

urls = [
    'https://lfs.aminer.cn/misc/ND-data/train_author.json',
    'https://lfs.aminer.cn/misc/ND-data/train_pub.json',
    'http://lfs.aminer.cn/misc/ND-data/sna_valid_author_raw.json',
    'http://lfs.aminer.cn/misc/ND-data/sna_valid_pub.json',
    'http://lfs.aminer.cn/misc/ND-data/sna_valid_example_evaluation_scratch.json',
    'http://lfs.aminer.cn/misc/ND-data/sna_valid_author_ground_truth.json',
    'http://lfs.aminer.cn/misc/ND-data/sna_test_author_raw.json',
    'http://lfs.aminer.cn/misc/ND-data/sna_test_pub.json'
]

names = [
    'data/train/train_author.json',
    'data/train/train_pub.json',
    'data/sna_data/sna_valid_author_raw.json',
    'data/sna_data/sna_valid_pub.json',
    'data/sna_data/sna_sna_valid_example_evaluation_scratch.json',
    'data/sna_data/sna_valid_author_ground_truth.json',
    'data/sna_test_data/sna_test_author_raw.json',
    'data/sna_test_data/test_pub_sna.json'
]

if __name__ == '__main__':
    def mk_if_nonexist(path):
        try:
            os.mkdir(path)
        except FileExistsError:
            pass

    for path in ['data', 'data/train', 'data/sna_data', 'data/sna_test_data',
                 'result', 'result_final', 'log', 'NewData', 'result_merge']:
        mk_if_nonexist(path)

    # it's slow (over 800MB), keep waiting
    for name, url in zip(names, urls):
        resp = req.get(url, headers=header)
        fp = open(name, 'wb')
        fp.write(resp.content)
        fp.close()
