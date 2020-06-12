import re
import os
import subprocess

for th in range(1, 9):
  print(f'{th} ', end = '', flush=True)
  for rate in [99, 90, 70, 50]:
    for skew in [0, 1]:
      o = subprocess.check_output(['./bench', str(th), str(rate), str(skew)]).decode()
      o = int(re.findall('throughput (.*?) operations/s', o)[0])
      print(f'& {o} ')
  print(r' \\ \hline')
