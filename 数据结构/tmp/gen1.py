from random import *
N = 500000
M = 1000000
print(N, M)
for _ in range(N):
    print(randint(-200, 200))
for _ in range(M):
    if randint(0, 1):
        print(0, randint(1, N), randint(-200, 200))
    else:
        l = randint(1, N)
        print(1, l, randint(l, N))
