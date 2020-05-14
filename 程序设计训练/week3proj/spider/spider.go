package main

import (
	"fmt"
	"github.com/PuerkitoBio/goquery"
	"golang.org/x/net/html/charset"
	"net/http"
	"os"
	"regexp"
	"strings"
	"sync"
	"time"
	"unicode/utf8"
)

var cnt = 0
var mutex = sync.Mutex{}
var que = Queue{}
var vis = map[string]string{}
var goodPattern, _ = regexp.Compile(`people\.com\.cn/n1`)
var badPattern, _ = regexp.Compile(`(sports)|(tv)|(cpc)`)

func getHref(doc *goquery.Document) {
	defer func() {
		recover()
	}()
	as := doc.Find("a")
	mutex.Lock()
	defer mutex.Unlock()
	for i, a := range as.Nodes {
		text := as.Eq(i).Text()
		if utf8.RuneCountInString(text) >= 10 {
			for _, h := range a.Attr {
				if h.Key == "href" && goodPattern.MatchString(h.Val) && !badPattern.MatchString(h.Val) {
					_, ok := vis[h.Val]
					text := as.Eq(i).Text()
					if !ok && !strings.Contains(text, "图") {
						vis[h.Val] = as.Eq(i).Text()
						que.Push(h.Val)
					}
				}
			}
		}
	}
}

func getContent(doc *goquery.Document, fileName string) {
	defer func() {
		err := recover()
		if err != nil {
			os.Remove(fileName)
		} else {
			cnt++
		}
	}()
	content := doc.Find("#rwb_zw")
	_ = content.Nodes[0]    // check size
	t := doc.Find(".box01") // find class
	_ = t.Nodes[0]          // check size
	file, _ := os.Create(fileName)
	defer file.Close()
	file.Write([]byte(strings.TrimSpace(t.Text())))
	file.Write([]byte("\n"))
	file.Write([]byte(strings.TrimSpace(content.Text())))
}

func getDoc(url string) *goquery.Document {
	defer func() {
		recover()
	}()
	req, _ := http.NewRequest("GET", url, nil)
	req.Header.Add("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:12.0) Gecko/20100101 Firefox/12.0")
	client := &http.Client{Timeout: time.Second}
	res, _ := client.Do(req)
	reader, _ := charset.NewReader(res.Body, "")
	doc, _ := goquery.NewDocumentFromReader(reader)
	return doc
}

func Bfs(root string, n int) {
	const CoroNum = 50
	que.Push(root)
	chs := [CoroNum]chan bool{}
	for i := range chs {
		chs[i] = make(chan bool)
	}
	for ; !que.Empty() && cnt < n; {
		currentCoro := CoroNum
		if currentCoro >= que.Len() {
			currentCoro = que.Len()
		}
		for i := 0; i < currentCoro; i++ {
			go func(finish chan bool) {
				mutex.Lock()
				url := que.Pop()
				fmt.Println(url, cnt)
				mutex.Unlock()
				doc := getDoc(url)
				getHref(doc)
				mutex.Lock()
				fileName := vis[url]
				mutex.Unlock()
				getContent(doc, fileName)
				finish <- true
			}(chs[i])
		}
		for i := 0; i < currentCoro; i++ {
			<-chs[i]
		}
	}
}

func main() {
	os.Mkdir("data", os.ModePerm)
	os.Chdir("data")
	beg := time.Now()
	Bfs("http://www.people.com.cn/", 20000)
	fmt.Println("Get 10000 pages in", time.Now().Sub(beg))
}

const BufSize = 1 << 20

type Queue struct {
	head, tail int
	data       [BufSize]string
}

func (q *Queue) Push(value string) {
	q.data[q.tail] = value
	q.tail = (q.tail + 1) & (BufSize - 1)
}

func (q *Queue) Pop() string {
	ret := q.data[q.head]
	q.head = (q.head + 1) & (BufSize - 1)
	return ret
}

func (q *Queue) Empty() bool {
	return q.head == q.tail
}

func (q *Queue) Len() int {
	return (q.tail - q.head) & (BufSize - 1)
}
