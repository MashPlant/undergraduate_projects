from itertools import chain, combinations


def powerset(xs):
    return chain.from_iterable(combinations(xs, n) for n in range(len(xs)+1))


l = [set('ABC'),
     set('ABCD'),
     set('BCE'),
     set('ACDE'),
     set('DE'), ]

P = powerset('ABCDE')
F = []
for x in powerset('ABCDE'):
    x = set(x)
    cnt = 0
    for li in l:
        cnt += x.issubset(li)
    if cnt >= 2:
        F.append((x, cnt))

closed = []
max = []
for x, cx in F:
    is_max, is_closed = True, True
    for y, cy in F:
        if x != y and x.issubset(y):
            is_max = False
            if cx == cy:
                is_closed = False
    s = ''.join(sorted(x))
    if is_max:
      max.append(s)
        # print(f'max: {x}')
    if is_closed:
      closed.append(s)
        # print(f'closed: {x}')
# print(powerset('ABCDE'))
print(max)
print(closed)