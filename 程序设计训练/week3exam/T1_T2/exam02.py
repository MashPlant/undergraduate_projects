from html.parser import HTMLParser

cities = []
colors = []


def _attr(alist, attrname):
    for attr in alist:
        if attr[0] == attrname:
            return attr[1]
    return None


name = ''


class Parser(HTMLParser):
    cnt = 0
    color = None
    last_attrs = {}

    def handle_starttag(self, tag, attrs):
        self.last_attrs = attrs
        if tag == 'tr':
            self.color = _attr(attrs, 'bgcolor')

    def handle_endtag(self, tag):
        if tag == 'tr':
            self.color = None

    def handle_data(self, data):
        ds = data.strip()
        if self.lasttag == 'title' and ds:
            global name
            name = ds
        if self.lasttag == 'title' or self.lasttag == 'style':
            return
        if _attr(self.last_attrs, 'style') != 'display:none' and ds:
            if self.cnt == 0:
                cities.append([ds])
                colors.append(self.color)
            else:
                cities[-1].append(ds)
            self.cnt += 1
            if self.cnt == 5:
                self.cnt = 0

    def handle_startendtag(self, tag, attrs):
        pass

    def handle_comment(self, data):
        pass


f = open('List of cities in China.html', 'r')
p = Parser()
raw = f.read()
p.feed(raw)
csv = open(name + '.csv', 'w')
for c in cities:
    csv.write(','.join(c))
    csv.write('\n')
color = open('color.csv', 'w')
for i, (ci, co) in enumerate(zip(cities, colors)):
    if co or i == 0:
        color.write(','.join(ci))
        color.write('\n')

import re
import datetime

body_p = re.compile(r'<tbody>(.*)</tbody>', re.S)
body = body_p.search(raw).group(1)

cnt = 0
cities = cities[1:]
while True:
    lst = body.find('</tr>')
    if lst == -1:
        break
    cities[cnt].append(body[:lst + 5])
    cnt += 1
    body = body[lst + 5:]

cities.sort(key=lambda x: datetime.datetime.strptime(x[4], '%Y-%m-%d'))

n_body = '<tbody>' + ''.join(c[5] for c in cities) + '</tbody>'

body = body_p.search(raw).span()
raw = raw[:body[0]] + n_body + raw[body[1]:]
open('Sorted Cities.html', 'w').write(raw)
