//
// Created by mashplant on 18-9-10.
//

#include <QThread>
#include <functional>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

struct Worker : QThread
{
    std::function<void(void)> work;

    Worker() = default;

    template<typename F, typename ...Args>
    Worker(F f, Args ...args) : QThread(nullptr), work([f, args...] { f(args...); })
    {}

protected:
    void run() override
    {
        qDebug() << "thread id" << currentThreadId();
        work();
    }
};

void work(int *first, int *last, int *dst, int k)
{
    qDebug() << '<' << QThread::currentThreadId() << ',' << first[0] % k << '>';
    qDebug() << '<' << QThread::currentThreadId() << ',' << first[1] % k << '>';
    qDebug() << '<' << QThread::currentThreadId() << ',' << first[2] % k << '>';
    for (; first != last; ++first)
        ++dst[*first % k];
}

int main(int argc, char **argv)
{
    ifstream fin("in.txt");
    vector<int> data;
    int x;
    while (fin >> x)
        data.push_back(x);
    int n = data.size(), k, n4 = n / 4;
    qDebug() << "please input k";
    cin >> k;
    std::vector<int> res[4];
    for (int i = 0; i < 4; ++i)
        res[i].resize(k);
    {
        Worker ws[4];
        for (int i = 0; i < 4; ++i)
            ws[i].work = bind(work, &data[i * n4], &data[(i + 1) * n4], res[i].data(), k);
        for (int i = 0; i < 4; ++i)
            ws[i].start();
        for (int i = 0; i < 4; ++i)
            ws[i].wait();
    }
    std::vector<int> merge(k);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < k; ++j)
            merge[j] += res[i][j];
    double ave = 0, std_var = 0;
    for (int i = 0; i < k; ++i)
        ave += merge[i];
    ave /= k;
    for (int i = 0; i < k; ++i)
        std_var += (merge[i] - ave) * (merge[i] - ave);
    std_var /= n;
    qDebug() << "ave =" << ave;
    qDebug() << "std var =" << sqrt(std_var);
}