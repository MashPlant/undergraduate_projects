from collections import defaultdict
from math import *

attrnames = ['', '', '身材', '发色', '年龄']

data = [
    (0, 1, '矮', '金色', '老人'),
    (0, 2, '高', '红色', '老人'),
    (0, 3, '高', '金色', '老人'),
    (0, 4, '矮', '金色', '成年'),
    (1, 5, '高', '黑色', '儿童'),
    (1, 6, '矮', '黑色', '老人'),
    (1, 7, '高', '黑色', '老人'),
    (1, 8, '高', '黑色', '成年'),
    (1, 9, '矮', '金色', '儿童'),
]


def i(a, b):
    if a == 0:
        return 0
    else:
        return a / b * log2(a / b)


def fmti(a, b):
    return f'\\frac{{{a}}}{{{b}}}\\log_2{{\\frac{{{a}}}{{{b}}}}}'


def id3(data, used):
    print(r'\item 剩余元素集合$\{' + str([x[1] for x in data])[1:-1] + r'\}$，剩余属性集合$\{' +
          ', '.join(f'\\text{{{attrnames[i]}}}' for i in range(2, len(data[0])) if i not in used) + r'\}$')
    unused = len(data[0]) - 2 - len(used)
    if unused == 0:
        print('由于没有可用属性，不再分类')
        return
    if all(x[0] == 0 for x in data) or all(x[0] == 1 for x in data):
        print('由于元素种类唯一，不再分类')
        return
    l = len(data)
    zeros = sum(x[0] == 0 for x in data)
    ent = -(i(zeros, l) + i(l - zeros, l))
    print(r'\begin{align*}')
    print(f'Info(D) &= -({fmti(zeros, l)} + {fmti(l - zeros, l)}) = {ent:.3f} \\\\')
    minent = 100
    for attr in range(2, len(data[0])):
        if attr not in used:
            choices = defaultdict(lambda: [[], []])
            for x in data:
                choices[x[attr]][x[0]].append(x)
            s = f'Info_{{\\text{{{attrnames[attr]}}}}} &= -('
            allent = 0.0
            for c, [z, o] in choices.items():
                (z, o) = (len(z), len(o))
                zo = z + o
                s += f'\\frac{{{zo}}}{{{l}}}({fmti(z, l)} + {fmti(o, l)}) + '
                allent -= zo / l * (i(z, l) + i(o, l))
            print(s[:-3] + f') = {allent:.3f}  \\\\')
            if allent < minent:
                minent = allent
                minattr = attr
                minchoices = choices
    print(r'\end{align*}')
    print(f'选择{attrnames[minattr]}，分出', end = '')
    for [z, o] in minchoices.values():
        print(r'$\{' + str([x[1] for x in z + o])[1:-1] + r'\}$和', end='')
    print()
    used = used.copy()
    used.add(minattr)
    for [z, o] in minchoices.values():
        id3(z + o, used)
    # used = used.copy().add(minattr)
    # print(s)
    # print(choices)
    # choices = set(x[attr] for x in data)
    # print(choices)


id3(data, set())
