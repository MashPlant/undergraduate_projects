# 实现的功能
	
1. 从人民网首页开始爬取数据并保存到文件；爬取的新闻总量在17000左右。
2. 设计新闻展示首页，在首页上可以进行搜索和翻页操作。
3. 设计新闻详情页，新闻详情页中包含该新闻的标题，时间，关键词，内容以及相关新闻推荐。点击关键词可以跳转到关键词搜索，点击新闻推荐可以跳转到对应新闻。
4. 关键词搜索时，对得到的新闻标题以及内容中的对应关键词进行高亮。搜索支持多关键词搜索（求交/求并均可），句子搜索，按照时间搜索。

   
# 实现效果

1. 新闻首页 [1.png](1.png)

2. 查询高亮 [2.png](2.png)
    [3.png](3.png)

3. 高级搜索 [4.png](4.png) (搜索内容为 "(习近平 AND 特朗普) OR (李克强 AND 特朗普) BEFORE 18-9-15 AFTER 18-1-1")


# 实现思路

## 爬虫部分

1. spider目录下的 [spider.py](../spider/spider.py)和[spider.go](../spider/spider.go)都可以直接运行，都会在spider目录下生成一个名称为data的子目录，二者爬取的数据格式相同。可以通过调整python函数bfs或者go函数Bfs的第二个参数来控制爬取的数量。
2. 爬虫是基于广度优先搜索来拓展网页。队列（实际上是circular buffer）的长度有限制避免浪费资源。利用两个语言内置的哈希表(dict/map)来查重。
3. 为了爬取效率，两个爬虫都利用了协程来并发爬取。python采用了标准库内置的async库，go采用了语言自带的协程。
4. 得到html文本之后进行解析。python采用了BeautifulSoup库，go采用了goquery库。从html中提取的信息包括：标题，时间，指向其它新闻的链接。
5. 最终效果是，go爬虫速度大致为2000页/分钟，python爬虫速度大致为600页/分钟。

## 数据处理部分

1. 采用python的jieba库对每条新闻进行分词。最终建立倒排索引，是一个词到Counter的映射，Counter是python collections包下的一个类，这里用来统计每个词在出现在哪个文章里以及出现次数。
2. 采用python gensim库进行文本相似性统计。采用的模型是TFIDF模型。对每个文章取相似度的前五保存下来。
3. 数据利用python pickle库保存到文件中，启动服务器时从文件中读取，减少查询时间。
4. 用户进行搜索时，首先利用[query_parser.py](../News/query_parser.py)进行解析。解析过程是基于递归下降。支持的操作包括AND，OR，BEFORE，AFTER，括号。对于查询中的每个句子，调用jieba分词再将其拆成一组词的OR。

## 网页部分

1. 设计了两个html模板，分别是[pages.html](../templates/pages.html)和[article.html](../templates/article.html)，分别对应于新闻首页和新闻内容。
2. 网页采用css进行美化。设计风格是google的material design风格。
3. 用户的输入信息都采用get的方式传给处理request的函数。
