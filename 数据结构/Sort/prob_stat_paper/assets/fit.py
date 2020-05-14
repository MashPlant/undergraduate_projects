import numpy as np
import matplotlib.pyplot as plt
from scipy import optimize
import sys

x = []
y = []
with open(sys.argv[1], 'r') as f:
    for line in f.readlines():
        a, b = map(float, line.split())
        x.append(a)
        y.append(b)
x = np.array(x)
y = np.array(y)

def R2(x, y, y_hat):
    y_bar = np.mean(y)
    SSE = np.sum((y_hat - y) ** 2)
    SST = np.sum((y - y_bar) ** 2)
    return 1 - SSE / SST

def f_nlogn(x, a, b):
    return a * np.log(x) * x + b * x

def f_n2(x, a, b):
    return a * x * x + b * x

f = f_nlogn if sys.argv[2] == 'nlogn' else f_n2

param, _ = optimize.curve_fit(f, x, y)
print(param)
print(R2(x, y, f(x, *param)))
plt.title(sys.argv[1])
plt.scatter(x, y)
plt.plot(x, f(x, *param))
plt.show()
