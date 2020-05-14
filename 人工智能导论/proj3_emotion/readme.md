提交的压缩包中没有任何数据，所以是不能直接运行的。我另外传了一个网盘的压缩包，地址：https://cloud.tsinghua.edu.cn/d/7c8013ec6c6a49d4851e/，这个压缩包中有处理好的输入数据和训练好的网络。如果只想要处理好的输入数据，重新训练一遍网络的话，需要从这个压缩包中的`word2vec.pk`和`data/sinanews/*`复制到对应的位置，然后执行 `python util.py`生成训练集/验证集/测试集的词向量表示。

`dnn.cpp`是我实现的一个全连接神经网络，采用C++编写，推荐的编译参数是`-Ofast -march=native -fopenmp dnn.cpp -o dnn`。
