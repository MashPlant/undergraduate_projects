#include <cstdio>
#include <cstdlib>
#include <cstring>

const int N = 100000 + 5;
char s[N], stk[N];

int main()
{
    int t;
    scanf("%d", &t);
    while (t--)
    {
        scanf("%s", s);
        int n = strlen(s), top = 0;
        bool ok = true;
        for (int i = 0; i < n; ++i)
        {
            if (s[i] == '(' || s[i] == '[' || s[i] == '{')
                stk[top++] = s[i];
            else
            {
                char ch = stk[--top];
                if ((s[i] == ')' && ch != '(') || (s[i] == ']' && ch != '[') || (s[i] == '}' && ch != '{'))
                {
                    ok = false;
                    break;
                }
            }
        }
        puts((ok && top == 0) ? "Yes" : "No");
    }
}