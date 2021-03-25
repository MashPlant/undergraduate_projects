from sklearn.cluster import KMeans
import numpy as np
l = [(1, 3), (1, 2), (2, 1), (2, 2), (2, 3), (3, 2), (5, 3),
     (4, 3), (4, 5), (5, 4), (5, 5), (6, 4), (6, 5)]

# init = [(0, 4), (6, 5)]
init = [(1.83333333, 2.16666667), (5, 4.14285714)]


def dis(a, b):
    return ((a[0]-b[0])**2+(a[1]-b[1])**2)**0.5


for x in l:
    print(x, end='：')
    for i in init:
        print(f'距离{i}为{dis(x, i):.3f}', end='，')
    print()
    # kmeans = KMeans(init=np.array([(0, 4), (6, 5)]), n_clusters=2,
    #                 random_state=0, verbose=1, n_init=1, max_iter=1).fit(l)
    # print(kmeans.labels_)
