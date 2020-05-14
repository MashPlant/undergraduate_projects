实现论文：ON CONSTRUCTING MINIMUM SPANNING TREES IN k-DIMENSIONAL SPACES AND RELATED PROBLEMS by ANDREW CHI-CHIH YAO，从而能在$o(n^2)$的时间内计算出一个二维平面上的图的最小生成树。

不过实际上这比用Delaunay三角剖分实现的二维最小生成树慢多了。这部分的代码我就是从网上找了一份(即`PlaneMST.cpp`，修改自http://trinkle.is-programmer.com/2015/7/1/delaunay-triangulation.100287.html)，然后改得更C++一点了而已(请注意不是更OOP一点，而是更C++一点)。


