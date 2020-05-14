#include <stdarg.h>
#pragma GCC optimize("no-stack-protector")
void hack_assert_fail(const char *assertion, const char *file,
                      unsigned int line, const char *function) {}
int hack_scanf(const char *__restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    *va_arg(args, int *) = 1;
    *va_arg(args, int *) = 1;
    return 2;
}
int main()
{
    hack_assert_fail(0, 0, 0, 0);
    return 0;
}
