import numpy as np


def concat_lists(ls):
    ret = []
    for l in ls: ret.extend(l)
    return ret


def normalized(v):
    length = np.sqrt(np.sum(v ** 2))
    return v if length == 0 else v / length
