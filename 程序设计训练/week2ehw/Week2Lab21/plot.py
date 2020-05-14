import matplotlib.pyplot as plt
import numpy as np

with open('result.txt', 'r') as f:
    y = [float(line) for line in f.readlines()]

plt.plot(y)
for idx in (np.argmax(y), np.argmin(y)):
    plt.plot(idx, y[idx], 'ks')
    plt.annotate('[%d,%f]' % (idx, y[idx]),
                 xytext=(idx, y[idx]), xy=(idx, y[idx]))
plt.xlabel('thread number')
plt.ylabel('time')
plt.show()
