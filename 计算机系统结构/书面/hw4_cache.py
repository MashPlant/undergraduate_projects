# 一个简单的cache模拟器，用来验证hw4的一道题的答案
import itertools


def push(stk, i):
    if i in stk:
        stk.remove(i)
    stk.append(i)


class Cache:
    SIZE = 12 * 1024 // 16
    WAY = 4
    WAY_SIZE = SIZE // WAY

    def __init__(self, lru):
        self.lru = lru
        self.lines = [[None] * Cache.WAY for _ in range(Cache.WAY_SIZE)]
        self.info = [[] for _ in range(Cache.WAY_SIZE)]
        self.miss = 0
        print(Cache.WAY_SIZE)

    def access(self, addr: int):
        tag, idx = divmod(addr, Cache.WAY_SIZE)
        line, stk = self.lines[idx], self.info[idx]
        free = None
        for i, b in enumerate(line):
            if b == tag:
                return push(stk, i)
            elif b is None:
                free = i
        self.miss += 1
        if free is None:
            free = stk[0] if self.lru else stk[Cache.WAY - 1]
        line[free] = tag
        push(stk, free)


def main():
    for lru in (True, False):
        c = Cache(lru)
        miss = 0
        for i in range(10):
            for j in range(1024):
                c.access(j)
            print(f'{i}: miss {c.miss - miss}')
            miss = c.miss
        print(f'tot: miss {miss}')


main()
