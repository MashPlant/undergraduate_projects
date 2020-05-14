import random
N = 2000000
print(N)
print('1234')
for i in range(N):
    if random.randint(0, 5) == 0 and i > 10:
        print(1)
    else:
        print(0, random.randint(0, 4294967295))
