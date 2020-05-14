#include <cpuid.h>
#include <stdint.h>
#include <cstdlib>

void cpuid(uint32_t func, uint32_t sub, uint32_t data[4])
{
    __cpuid_count(func, sub, data[0], data[1], data[2], data[3]);
}

int main()
{
    uint32_t data[4];
    char str[48];
    for (int i = 0; i < 3; ++i)
    {
        cpuid(0x80000002 + i, 0, data);
        for (int j = 0; j < 4; ++j)
            reinterpret_cast<uint32_t *>(str)[i * 4 + j] = data[j];
    }
    exit(data[8]);
}