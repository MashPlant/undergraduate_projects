import asyncio
import requests
import os
import bs4
from bs4 import BeautifulSoup
from urllib import request
from collections import deque
from typing import List, Tuple, NoReturn, Deque, Dict
import re

html_pattern = re.compile(r'<[^>]+>', re.S)
target_folder = 'data2'

good_pattern = re.compile(r'people\.com\.cn/n1')
bad_pattern = re.compile('(sports)|(tv)|(cpc)')


def get_content(soup: BeautifulSoup, file_name: str) -> NoReturn:
    try:
        text: bs4.element.Tag = soup.find_all(id='rwb_zw')[0]
        date: bs4.element.Tag = soup.find_all(class_='box01')[0]
    except Exception as e:
        raise e  # rethrow
    with open(os.path.join(target_folder, file_name), 'w', encoding='UTF-8') as f:
        f.write(date.text.strip())
        f.write('\n')
        for content in text.contents:
            para = html_pattern.sub('', str(content)).strip()
            if para and para.find('showPlayer') == -1:
                f.write(para)
                f.write('\n')


def get_href(soup: BeautifulSoup) -> Tuple[List[str], List[str]]:
    href_list: List[str] = []
    name_list: List[str] = []
    href: bs4.element.Tag
    for href in soup.find_all(name='a'):
        if len(href.contents) == 0 or len(href.contents[0]) < 12:
            continue
        if 'href' in href.attrs and href['href'].find('http') != -1 \
                and good_pattern.search(href['href']) and not bad_pattern.search(href['href']):
            href_list.append(href['href'])
            name_list.append(href.contents[0])
    return href_list, name_list


def bfs(root: str, n):
    que = deque([root], maxlen=n * 200)
    vis = {root: 'ROOT'}
    loop = asyncio.get_event_loop()

    async def work(url: str):
        try:
            nonlocal n
            print(url, n)
            urlopen = lambda u: request.urlopen(u, timeout=1)
            response = await loop.run_in_executor(None, urlopen, url)
            soup = BeautifulSoup(response.read(), 'lxml')
            href_list, name_list = get_href(soup)
            # print(href_list)
            for href, name in zip(href_list, name_list):
                if href not in vis:
                    vis[href] = name
                    que.append(href)
                    # print('appending...')
            get_content(soup, vis[url])
            n -= 1
        except Exception as e:
            pass
            # print(url, e)

    loop = asyncio.get_event_loop()
    while n > 0 and que:
        print('len of que', len(que))
        task_num = min(len(que), 50)
        tasks = [work(que.popleft()) for _ in range(task_num)]
        loop.run_until_complete(asyncio.wait(tasks))
    loop.run_until_complete(asyncio.sleep(0))
    loop.close()


def mkdir(path: str) -> NoReturn:
    path = path.strip().rstrip("\\")
    if not os.path.exists(path):
        os.makedirs(path)
    else:
        print('already exist')


mkdir(target_folder)
bfs('http://www.people.com.cn/', 20000)
