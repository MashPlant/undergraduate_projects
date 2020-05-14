#include <stack>
#include <cstdio>
#include <cstdlib>
#include <cctype>

double eval(const char *s)
{
    static unsigned char priority[128];
    struct _
    {
        _(unsigned char *p)
        {
            p['('] = 1;
            p['+'] = p['-'] = 2;
            p['*'] = p['/'] = 3;
        }
    };
    static _ _(priority);
    std::stack<double> opr;
    std::stack<char> opt;
    auto calc = [&]()
    {
        char cur = opt.top();
        opt.pop();
        double r = opr.top(), l = (opr.pop(), opr.top());
        opr.pop();
        switch (cur)
        {
            case '+':
                opr.push(l + r);
                break;
            case '-':
                opr.push(l - r);
                break;
            case '*':
                opr.push(l * r);
                break;
            case '/':
                opr.push(l / r);
                break;
        }
    };
    for (const char *it = s; *it;)
    {
        if (isdigit(*it))
        {
            char *tmp = const_cast<char *>(it);
            opr.push(strtod(it, &tmp));
            it = tmp;
        }
        else if (*it == '(')
            opt.push(*it++);
        else if (*it == ')')
        {
            while (opt.top() != '(')
                calc();
            opt.pop();
            ++it;
        }
        else
        {
            while (!opt.empty() && priority[opt.top()] >= priority[*it])
                calc();
            opt.push(*it++);
        }
    }
    while (!opt.empty())
        calc();
    return opr.top();
}

int main()
{
    printf("%lf\n", eval("(1+2)*3/4*5-6"));
}