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


import os

os.chdir('Dj')
f = open('List of cities in China.html', 'r')
p = Parser()
p.feed(f.read())
cities = cities[1:]
