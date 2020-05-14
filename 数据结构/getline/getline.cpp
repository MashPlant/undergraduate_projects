#include <cstdio>
#include <cstring>
#include <cctype>

struct IO
{
    char *p_in = in, *p_out = out;
    char in[1 << 27], out[1 << 27];
    IO() { fread(in, 1, sizeof in, stdin); }
    ~IO() { fwrite(out, 1, p_out - out, stdout); }
    int read(char *s) // consume '\n', but not put it in s
    {
        while (isspace(*p_in))
            ++p_in;
        char *n = strchr(p_in, '\n');
        int len = n - p_in;
        memcpy(s, p_in, len), s[len] = '\0';
        p_in = n + 1;
        return len;
    }
    operator int()
    {
        unsigned v = 0;
        while (*p_in < '0')
            p_in++;
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return v;
    }
    void print(const char *s)
    {
        print(s, strlen(s));
    }
    void print(const char *s, int len)
    {
        memcpy(p_out, s, len);
        p_out += len;
        *p_out++ = '\n';
    }
} io;
char s[1 << 18];

int main()
{
    int n = io;
    while (n--)
    {
        int len = io.read(s);
        io.print(s, len);
    }
}