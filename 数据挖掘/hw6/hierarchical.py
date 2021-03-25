SIM = [[1.00, 0.10, 0.41, 0.55, 0.35],
       [0.10, 1.00, 0.64, 0.47, 0.98],
       [0.41, 0.64, 1.00, 0.44, 0.85],
       [0.55, 0.47, 0.44, 1.00, 0.76],
       [0.35, 0.98, 0.85, 0.76, 1.00]]

clusters = [[0], [1], [2], [3], [4]]
# tree = [[[0]], [[1]], [[2]], [[3]], [[4]]]
tree = [0, 1, 2, 3, 4]


def min_sim(l1, l2):
    return min(SIM[x][y] for x in l1 for y in l2)


def max_sim(l1, l2):
    return max(SIM[x][y] for x in l1 for y in l2)


def flatten(l):
    for x in l:
        if isinstance(x, list):
            yield from flatten(x)
        else:
            yield x


for _ in range(4):
    m = 0
    thei, thej = None, None
    for i in range(len(clusters)):
        for j in range(i):
            d = min_sim(clusters[i], clusters[j])
            if d > m:
                m = d
                thei, thej = i, j
    # thei > thej
    tmp = clusters.pop(thei)
    print(f'{m} :merge {tmp} {clusters[thej]}')
    clusters[thej].extend(tmp)
    tmp = tree.pop(thei)
    tree[thej] = [tree[thej], tmp]


idx = 0


def show(l):
    global idx
    if isinstance(l, int):
        print(f'{idx} [label="P{l + 1}"]')
    else:
        print(
            f'{idx} [label="{", ".join(f"P{x + 1}" for x in sorted(flatten(l)))}"]')
        orig = idx
        for x in l:
            idx += 1
            print(f'{orig} -> {idx}')
            show(x)


show(tree[0])
