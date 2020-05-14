#include "Matrix.h"
#include <stdexcept>
#include <fstream>
#include <cstring>

//更好的选择是std::make_unique
//但是它是C++14才加入的,为了避免您的编译器不支持,这里还是用std::unique_ptr的构造函数
Matrix::Matrix(int r, int c) : r(r), c(c), data(new int[r * c])
{
    memset(data.get(), 0, sizeof(int) * r * c);
}

Matrix::Matrix(const char *file)
{
    load(file);
}

void Matrix::load(const char *file)
{
    std::ifstream fin(file);
    fin >> r >> c;
    data.reset(new int[r * c]);
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            fin >> data[i * c + j];
}

template <typename OS>
void Matrix::output(OS &&os) const
{
    os << r << ' ' << c << '\n';
    for (int i = 0; i < r; ++i, os << '\n')
        for (int j = 0; j < c; ++j)
            os << data[i * c + j] << ' ';
    os << std::flush;
}

void Matrix::display() const
{
    output(std::cout); //std::cout是左值,可以被OS &&os绑定
}

void Matrix::save(const char *file) const
{
    output(std::ofstream(file)); //std::ofstream(file)是右值,可以被OS &&os绑定
}

Matrix Matrix::multiply(const Matrix &rhs) const
{
    if (c != rhs.r) //检查矩阵是否可乘
        throw(std::invalid_argument("Matrix shape not fit"));
    Matrix ret(r, rhs.c);
    /*
    下面的计算在逻辑上等价于
    for (int i = 0; i < a.r; ++i)
        for (int k = 0; k < a.c; ++k)
        {
            int tmp = a[i][k];
            for (int j = 0; j < b.c; ++j)
                c[i][j] += tmp * b[k][j]; 
        }
    代码做了一点简单的优化
    */
    for (int i = 0; i < r; ++i)
        for (int k = 0; k < c; ++k)
        {
            int tmp = data[i * c + k];
            for (int *pRet = ret.data.get() + i * ret.c,
                     *pRhs = rhs.data.get() + k * rhs.c,
                     *pEnd = pRet + rhs.c;
                 pRet != pEnd; ++pRet, ++pRhs)
                *pRet += tmp * *pRhs;
        }
    return ret;
}