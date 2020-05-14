import random
N = 1000000
limit = 10300000
print(N, limit)

l = list(range(0, N))
random.shuffle(l)

for i in l:
    print(i, end=' ')
