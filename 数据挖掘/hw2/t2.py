import numpy as np
from scipy import stats
import pandas as pd
import matplotlib.pyplot as plt

l = [9.0, 26.5, 7.8, 17.8, 31.4, 26.5, 27.4, 26.5, 34.6, 43.0, 28.8, 33.4, 27.4, 34.1, 32.9, 41.2]

print('mean:', np.mean(l))
print('median:', np.median(l))
print('mode:', stats.mode(l)[0][0])

print('min:', np.min(l))
print('25th:', np.percentile(l, 25))
print('median:', np.median(l))
print('75th:', np.percentile(l, 75))
print('max:', np.max(l))

df = pd.DataFrame(l)
# 箱型图的绘制原则是：
# 如果最大值/最小值不越过上边缘/下边缘，那么线只延伸到最大值/最小值
# 否则，最大值/最小值是离群点，线只延伸到上边缘/下边缘
df.boxplot()
plt.show()
 
# l=[8408,1374,1872,8879,2459,11413,608,14138,6452,1850,2818,1356,10498,7478,4019,4341,739,2127,3653,5794,8305]
 
 
# def fiveNumber(nums):
# 	#五数概括 Minimum（最小值）、Q1、Median（中位数、）、Q3、Maximum（最大值）
# 	Minimum=min(nums)
# 	Maximum=max(nums)
# 	Q1=np.percentile(nums,25)
# 	Median=np.median(nums)
# 	Q3=np.percentile(nums,75)
	
# 	IQR=Q3-Q1
# 	lower_limit=Q1-1.5*IQR #下限值
# 	upper_limit=Q3+1.5*IQR #上限值
	
# 	return Minimum,Q1,Median,Q3,Maximum,lower_limit,upper_limit

# print(fiveNumber(l))
# df = pd.DataFrame(l)
# df.boxplot()
# plt.show()