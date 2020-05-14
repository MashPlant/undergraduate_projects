from django.shortcuts import render
from django.http import HttpRequest
from django.http import HttpResponse, Http404, HttpResponseRedirect
from . import data
import datetime

cities = data.cities


def init(request):
    return HttpResponse("""
    <form method="get" action="/city/">
        <input type="text" name="province">
        <input type="submit" value="query city">
    </form>
    <br>
    <form method="get" action="/time/">
        <input type="text" name="beg">
        <input type="text" name="end">
        <input type="submit" value="query time">
    </form>
    """)


def city(request):
    # 页面显示所有该省的所有城市的英文名和中文名以及查询到的城市数量。
    pro = request.GET['province']
    res = []
    for i, c in enumerate(cities):
        if c[2] == pro:
            res.append([c[0], c[1], i])
    s = f'<h1>Found {len(res)} cities in province {pro}</h1>'
    s += '<ul>'
    for r in res:
        s += f'<li><a href="/profile/{r[2]}">{r[0]} {r[1]}</a></li>\n'
    s += '</ul>'
    return HttpResponse(s)


def time(request):
    beg = datetime.datetime.strptime(request.GET['beg'], '%Y-%m-%d')
    end = datetime.datetime.strptime(request.GET['end'], '%Y-%m-%d')
    res = []
    for i, c in enumerate(cities):
        if beg <= datetime.datetime.strptime(c[4], '%Y-%m-%d') <= end:
            res.append([c[0], c[1], i])
    s = f'<h1>Found {len(res)} cities between {request.GET["beg"]} and {request.GET["end"]}</h1>'
    s += '<ul>'
    for r in res:
        s += f'<li><a href="/profile/{r[2]}">{r[0]} {r[1]}</a></li>\n'
    s += '</ul>'
    return HttpResponse(s)


def profile(request, index):
    index = int(index)
    c = cities[index]
    s = '<h1>City Profile</h1>'
    s += f'{c[0]},{c[1]},{c[2]},{c[3]},{c[4]}<br>'
    res = []
    for i, c_ in enumerate(cities):
        if c_[3] == c[0]:
            res.append([c_[0], c_[1], i])
    if res:
        s += '<h2>Children Cities</h2>'
        s += '<ul>'
        for r in res:
            s += f'<li><a href="/profile/{r[2]}">{r[0]} {r[1]}</a></li>\n'
        s += '</ul>'
    return HttpResponse(s)
