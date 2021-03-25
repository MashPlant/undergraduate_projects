# WhoIsWho-DataMining-Competition
WhoIsWho -- AMiner manually-labeled name disambiguation competition 2020 solution

现在仓库中的代码基于https://biendata.com/models/category/3679/L_notebook/ 修改而来，这个网页中依次有六个代码片段，对应了当前仓库中的六个`.py`文件。其中`final_result.py`和`make_cluster.py`经我(李晨昊)做了比较多的修改，基本语义没有变，但是不保证输出完全一样，因为他的代码中有些地方依赖于类似字典的遍历顺序这样implementation-defined的东西。



修改了运行方法和LGB-bin分支保持一致，代码移动到src文件夹中，运行全过程`python -m src.run`

单独运行某一步可以用以下命令

```
python -m src.data_process
python -m src.gsdmm
python -m src.make_cluster
python -m src.merge
```

单独运行才会把中间过程保存成文件，运行run.py则不会保存中间结果