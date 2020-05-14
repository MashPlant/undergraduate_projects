import random
ranint = random.randint
N = 1200
K = 2000000
L = -1000
H = 999
print(N, N, K)

for _ in range(N*N):
    print(ranint(L, H), end=' ')
print()
for _ in range(K):
    op = ranint(0, 1)
    print(op, end=' ')
    if op == 0:
        print(ranint(1, N), ranint(1, N), ranint(L, H), end=' ')
    else:
        x1 = ranint(1, N)
        y1 = ranint(1, N)
        x2 = ranint(x1, N)
        y2 = ranint(y1, N)
        print(x1, y1, x2, y2, end=' ')
    print()
