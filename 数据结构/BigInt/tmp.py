import time
import random
a = random.randint(int('1'+'0'*10000), int('2'+'0'*10000))
b = random.randint(int('1'+'0'*100), int('2'+'0'*100))


def timeit(f):
    beg = time.time()
    f()
    print(time.time() - beg)


timeit(lambda: (a//b))
