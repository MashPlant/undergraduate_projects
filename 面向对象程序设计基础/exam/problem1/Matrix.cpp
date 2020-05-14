#include "Matrix.h"
#include <stdexcept>
#include <fstream>
#include <cstring>

void Matrix::swap(Matrix &rhs)
{
    std::swap(r, rhs.r);
    std::swap(c, rhs.c);
    std::swap(data, rhs.data);
}

Matrix::Matrix(const Matrix &rhs) : r(rhs.r), c(rhs.c)
{
    data.reset(new int[r * c]);
    memcpy(data.get(), rhs.data.get(), r * c * sizeof(int));
}

Matrix &Matrix::operator=(const Matrix &rhs)
{
    if (this != &rhs)
    {
        Matrix tmp(rhs);
        swap(tmp);
    }
    return *this;
}

Matrix::Matrix(int r, int c) : r(r), c(c), data(new int[r * c])
{
    memset(data.get(), 0, sizeof(int) * r * c);
}

int *Matrix::operator[](int index)
{
    return data.get() + index * c;
}

const int *Matrix::operator[](int index) const
{
    return data.get() + index * c;
}

Matrix Matrix::operator+(const Matrix &rhs) const
{
    if (c != rhs.c || r != rhs.r)
        return Matrix(0, 0);
    Matrix ret(r, c);
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            ret[i][j] = (*this)[i][j] + rhs[i][j];
    return ret;
}

bool Matrix::operator<=(const Matrix &rhs) const
{
    if (c != rhs.c || r != rhs.r)
        return false;
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            if ((*this)[i][j] > rhs[i][j])
                return false;
    return true;
}

Matrix Matrix::operator*(const Matrix &rhs) const
{
    if (c != rhs.r) //检查矩阵是否可乘
        return Matrix(0, 0);
    Matrix ret(r, rhs.c);
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

std::istream &operator>>(std::istream &is, Matrix &mat)
{
    is >> mat.r >> mat.c;
    mat.data.reset(new int[mat.r * mat.c]);
    for (int i = 0; i < mat.r; ++i)
        for (int j = 0; j < mat.c; ++j)
            is >> mat[i][j];
    return is;
}

std::ostream &operator<<(std::ostream &os, const Matrix &mat)
{
    os << mat.r << ' ' << mat.c << '\n';
    for (int i = 0; i < mat.r; ++i, os << '\n')
        for (int j = 0; j < mat.c; ++j)
            os << mat[i][j] << ' ';
    os << std::flush;
    return os;
}
