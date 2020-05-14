#include <ctime>
#include <functional>
#include <iostream>
using namespace std;

int main()
{
    struct F
    {
        int operator()(int x)
        {
            if (x == 1 || x == 2)
                return 1;
            return F()(x - 1) + F()(x - 2);
        }
    };
    clock_t beg = clock();
    F f;
    cout << f(39) << endl;
    cout << clock() - beg << endl;

    beg = clock();

    std::function<int(int)> fib = [&](int x) -> int {
        if (x == 1 || x == 2)
            return 1;
        return fib(x - 1) + fib(x - 2);
    };
    cout << fib(39) << endl;
    cout << clock() - beg << endl;
    //lambda的递归必须用std::function来包装,其调用会涉及到虚函数
    //但是重载()的struct无此限制,上面的速度测试中,无论是否开启O2,都是struct更快
}
