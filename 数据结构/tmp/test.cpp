#include <algorithm>

int main()
{
    int a[10] = {1, 2, 3};
    std::nth_element(a, a + 5, a + 10);
}