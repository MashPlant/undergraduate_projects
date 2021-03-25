import nltk
import json
import re
from translate import Translator
from multiprocessing import Pool

# 一些全局变量，避免重复加载数据
train_author = json.load(open("./data/train/train_author.json"))

path_train = "./preprocess_pubs/cleaned_train_pubs.json"
path_test = "./preprocess_pubs/cleaned_valid_pubs.json"

try:
    # 预处理后的训练集和验证集的paper信息
    pub_train = json.load(open(path_train))
    pub_test = json.load(open(path_test))
except:
    # 这并没有什么关系，按照预期，使用者应该在运行过pre.py后再运行别的脚本，所以真正用到这些数据的时候必然已经加载好了
    print("这是第一次运行pre.py，清洗过的数据尚未生成")

punctuation = r'[a-z~!@#$%\^&*()_+`0-9\-=\[\]{}\\\\":;/?>.<,|~！￥……（）—｛｝、“”‘’：；？》。《，]'


# 检查字符串是否含中文/日文，
# 语言编号0表示只含英文，1表示含中文，2表示含日文(都含时为2)
# is_name参数表示这个字符串是不是来自姓名字段
# lang_flag配合返回值，用来统计遇见过的最大的语言编号
def translate(string, lang, is_name=False):
    chinese = False
    for ch in string:
        if u"\u3040" <= ch <= u"\u309f" or u"\u30a0" <= ch <= u"\u30ff":
            if is_name:
                string = re.sub(punctuation, "", string)
            string = Translator(from_lang="jp", to_lang="en").translate(string)
            return max(lang, 2), string
        if u"\u4e00" <= ch <= u"\u9fff":
            chinese = True
    if chinese:
        if is_name:
            string = re.sub(punctuation, "", string)
        string = Translator(from_lang="zh", to_lang="en").translate(string)
        return max(lang, 1), string
    return lang, string


stop_word = nltk.corpus.stopwords.words("english")
wnl = nltk.stem.WordNetLemmatizer()
lemmatize_filter = {".", "LS", "IN", "EX", "CC", "DT", "MD", "PDT", "SYM", "TO", "WDT", "WP", "UH", "CD"}


# 词形还原、转为小写
def word_lemmatize(context):
    context = re.sub("[^a-zA-Z]", " ", context).split()
    for word, tag in nltk.pos_tag(context):
        if tag in lemmatize_filter or len(word) < 2:
            continue
        if tag.startswith("NN"):
            _word = wnl.lemmatize(word, pos="n")
        elif tag.startswith("VB"):
            _word = wnl.lemmatize(word, pos="v")
        elif tag.startswith("JJ"):
            _word = wnl.lemmatize(word, pos="a")
        elif tag.startswith("R"):
            _word = wnl.lemmatize(word, pos="r")
        else:
            _word = word
        if _word not in stop_word:
            yield _word


def none_or(a, b):
    return a if a else b


# 收集每篇paper的一些特征，返回值为长度为9的列表:
# [0] = 关键词数; useful to cross
# [1] = 期刊名长度; useful to cross
# [2] = 年份是否缺失;
# [3] = 摘要长度; useful to cross
# [4] = 作者数; useful to cross
# [5] = 作者机构缺失数; useful to cross
# [6] = 缺失作者名数;
# [7] = 是否含中文;
# [8] = 是否含日文;
def process_paper(paper):
    feature = [0] * 9
    paper["keywords"] = none_or(paper.get("keyword"), [])
    feature[0] = len(paper["keywords"])
    paper["venue"] = none_or(paper.get("venue"), "")
    if not paper.get("year"):  # None或""
        paper["year"] = -1
        feature[2] = 1
    paper["abstract"] = none_or(paper.get("abstract"), "")
    paper["title"] = none_or(paper.get("title"), "")
    feature[4] = len(paper["authors"])
    for au in paper["authors"]:
        if not au.get("org"):  # None或""
            feature[5] = 1
            au["org"] = ""
        if au["name"] == "":
            feature[6] = 1

    lang = 0
    lang, paper["title"] = translate(paper["title"], lang)
    lang, paper["abstract"] = translate(paper["abstract"], lang)
    lang, paper["venue"] = translate(paper["venue"], lang)
    for i, kw in enumerate(paper["keywords"]):
        lang, paper["keywords"][i] = translate(kw, lang)
    for au in paper["authors"]:
        lang, au["name"] = translate(au["name"], lang, True)
        lang, au["org"] = translate(au["org"], lang)

    feature[1] = len(paper["venue"])
    feature[3] = len(paper["abstract"])
    feature[7] = int(lang == 1)
    feature[8] = int(lang == 2)

    paper["title"] = " ".join(word_lemmatize(paper["title"]))
    paper["venue"] = " ".join(word_lemmatize(paper["venue"]))
    paper["abstract"] = " ".join(word_lemmatize(paper["abstract"]))
    paper["keywords"] = " ".join(x for kw in paper["keywords"] for x in word_lemmatize(kw))
    for i in range(min(100, len(paper["authors"]))):  # 只取前100个
        au = paper["authors"][i]
        au["org"] = list(word_lemmatize(au["org"].lower()))
        au["name"] = re.sub("[^a-z]", " ", au["name"].lower()).split()

    return {
        "title": paper["title"],
        "venue": paper["venue"],
        "keywords": paper["keywords"],
        # 只取前100个
        "authors": [paper["authors"][i] for i in range(min(100, len(paper["authors"]))) if paper["authors"][i]["name"]],
        "year": paper["year"],
        "abstract": paper["abstract"],
        "other_feature": feature,
    }


def pre_process_pubs(pubs, save_path):
    kv = list(pubs.items())
    processed_papers = Pool(4).map(process_paper, (v for k, v in kv))
    json.dump({k: v for (k, _), v in zip(kv, processed_papers)}, open(save_path, "w"), indent=4)


if __name__ == "__main__":
    pre_process_pubs(json.load(open("./data/v2/valid/sna_valid_pub.json")), path_test)
    pre_process_pubs(json.load(open("./data/train/train_pub.json")), path_train)
