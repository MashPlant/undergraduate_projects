import random
N = 16777216
print(''.join('01'[random.randint(0, 1)] for _ in range(0, N)))
