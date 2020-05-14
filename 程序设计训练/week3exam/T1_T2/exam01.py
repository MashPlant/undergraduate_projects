import sys
import re
from fractions import Fraction

if len(sys.argv) < 2:
    print('please enter a arg')
    sys.exit(1)

of = None
if len(sys.argv) == 3:
    of = open(sys.argv[2], 'w')

pat = re.compile(r'(\d+)')

try:
    f = open(sys.argv[1], 'r')
except:
    print('no such file')
    sys.exit(1)

for ln in f.readlines():
    ln = pat.sub(r'Fraction(\1)', ln)
    if of:
        of.write(str(eval(ln)))
        of.write('\n')
    else:
        print(eval(ln))
