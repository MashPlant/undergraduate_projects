import numpy as np
import matplotlib.pyplot as plt
from sklearn.manifold import TSNE
from sklearn.decomposition import PCA

ls = open('100_word_vector.txt').readlines()
data = np.ndarray((len(ls), 100))
for i in range(len(ls)):
  ls[i] = ls[i].split()
  data[i] = np.fromiter(map(float, ls[i][1:]), np.float)
seed = 19260817
for method in (PCA(n_components = 2, random_state = seed), TSNE(n_components = 2, random_state = seed)):
  transformed = method.fit_transform(data)
  _, ax = plt.subplots()
  ax.scatter(transformed[:, 0], transformed[:, 1])
  for (x, y), l in zip(transformed, ls):
    ax.text(x, y, l[0])
  ax.set_aspect('equal')
  ax.grid(True, which = 'both')
  ax.axhline(y = 0, color = 'k')
  ax.axvline(x = 0, color = 'k')
  plt.show()