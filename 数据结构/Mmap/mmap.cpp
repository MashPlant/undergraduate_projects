#include "common.h"
#include <sys/mman.h>
#include <sys/stat.h>

int main()
{
    auto beg = now();
    struct stat st;
    int fn = fileno(stdin);
    fstat(fn, &st);
    char *a = (char *)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fn, 0);
    int s = 0;
    for (int i = 0; i < N; ++i)
        s += a[i];
    if (rand() == 0)
        printf("%d\n", s);
    fprintf(stderr, "%.2lfms\n", 1000 * duration<double>(now() - beg).count());
}