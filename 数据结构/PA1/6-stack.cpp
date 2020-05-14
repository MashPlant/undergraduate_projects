#include <cstdio>
#include <cctype>
#include <cstring>
#include <cassert>

const int N = 7200000 + 5;

struct IO
{
    char *p_in = in, *p_out = out;
    char in[3200000 + 4000000 * 6 + 2333], out[20 * 1024 * 1024 + 5];

    IO()
    {
#ifndef _OJ_
        freopen("in.txt", "r", stdin);
#endif
        fread(in, 1, sizeof in, stdin);
    }

    ~IO()
    {
        fwrite(out, 1, p_out - out, stdout);
    }

    char next_char()
    {
        char ch;
        while (isspace(ch = *p_in++))
            ;
        return ch;
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

    void println(char ch)
    {
#ifdef _OJ_
        *p_out++ = ch, *p_out++ = '\n';
#endif
    }
} io;

struct Q
{
    int inv, head = N, tail = N;
    char data[N * 2];

    void push_front(int x)
    {
        data[inv ? tail++ : --head] = x;
    }

    void push_back(int x)
    {
        data[inv ? --head : tail++] = x;
    }

    bool empty()
    {
        return head == tail;
    }

    int pop_front()
    {
        return data[inv ? --tail : head++];
    }

    int pop_back()
    {
        return data[inv ? head++ : --tail];
    }

    void rev()
    {
        inv ^= 1;
    }
} q;

template <typename T>
void swap(T &a, T &b)
{
    T c = a;
    a = b;
    b = c;
}
char stk[N * 2];
constexpr char *stkl = stk, *stkr = stk + N;
bool isl(char *p)
{
    return p < stkr;
}

bool isr(char *p)
{
    return p >= stkr;
}

void move_left(char *&p1, char *&p2)
{
    bool ok;
    if (isl(p1))
    {
        if (p1 == stkl)
            ok = false;
        else
            ok = true, q.push_front(*--p1);
    }
    else
    {
        if (q.empty())
        {
            if (p2 == stkl) // all empty
                ok = false;
            else
                ok = true, q.push_front(*--p2), swap(p1, p2);
        }
        else
            ok = true, *p1++ = q.pop_back();
    }
    io.println(ok ? 'T' : 'F');
}

void move_right(char *&p1, char *&p2)
{
    bool ok;
    if (isr(p1))
    {
        if (p1 == stkr)
            ok = false;
        else
            ok = true, q.push_back(*--p1);
    }
    else
    {
        if (q.empty())
        {
            if (p2 == stkr) // all empty
                ok = false;
            else
                ok = true, q.push_back(*--p2), swap(p1, p2);
        }
        else
            ok = true, *p1++ = q.pop_front();
    }
    io.println(ok ? 'T' : 'F');
}

int main()
{
    {
        char ch;
        while (!isspace(ch = *io.p_in++))
            q.push_back(ch);
    }
    int t = io;
    char *topl = stkl, *topr = stkr;
    char op, w, val;
    while (t--)
    {
        op = io.next_char();
        switch (op)
        {
        case '<':
            w = io.next_char();
            move_left(w == 'L' ? topl : topr, w == 'L' ? topr : topl);
            break;
        case '>':
            w = io.next_char();
            move_right(w == 'L' ? topl : topr, w == 'L' ? topr : topl);
            break;
        case 'I':
        {
            w = io.next_char(), val = io.next_char();
            io.println('T');
            char *&p1 = w == 'L' ? topl : topr;
            char *&p2 = w == 'L' ? topr : topl;
            if (isl(p1))
                *p1++ = val;
            else
            {
                if (q.empty())
                    *p2++ = val;
                else
                    q.push_back(val);
            }
        }
        break;
        case 'D':
        {
            w = io.next_char();
            char *&p1 = w == 'L' ? topl : topr;
            char *&p2 = w == 'L' ? topr : topl;
            if (isl(p1))
            {
                if (q.empty())
                {
                    if (p2 == stkr) // all empty
                        io.println('F');
                    else
                    {
                        io.println('T');
                        --p2;
                    }
                }
                else
                {
                    io.println('T');
                    q.pop_front();
                }
            }
            else
            {
                if (p1 == stkr)
                    io.println('F');
                else
                {
                    io.println('T');
                    --p1;
                }
            }
        }
        break;
        case 'R':
            if (isr(topl) || q.empty())
                io.println('F');
            else
            {
                io.println('T');
                q.rev();
            }
            break;
        case 'S':
        {
            char *p1 = isl(topl) ? topl : topr;
            char *p2 = isl(topl) ? topr : topl;
            memcpy(io.p_out, stkl, sizeof(char) * (p1 - stkl)), io.p_out += p1 - stkl;
            if (q.inv)
                for (char *it = q.tail + q.data - 1; it >= q.head + q.data; --it)
                    *io.p_out++ = *it;
            else
                memcpy(io.p_out, q.data + q.head, sizeof(char) * (q.tail - q.head)), io.p_out += q.tail - q.head;
            for (char *it = p2 - 1; it >= stkr; --it)
                *io.p_out++ = *it;
            *io.p_out++ = '\n';
        }
        break;
        }
    }
}
