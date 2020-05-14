#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
#include <memory>

class Matrix
{
  //这里不使用二重指针,而是直接申请一个r*c的动态数组
  //这样内存的连续性更好,速度会快一些
  int r, c;
  std::unique_ptr<int[]> data;

  void swap(Matrix &rhs);

public:
  Matrix(const Matrix &rhs);

  Matrix &operator=(const Matrix &rhs);

  Matrix() = default;

  ~Matrix() = default;

  //生成r行c列的矩阵(元素全置为0)
  Matrix(int r, int c);

  Matrix operator*(const Matrix &rhs) const;

  Matrix operator+(const Matrix &rhs) const;

  bool operator<=(const Matrix &rhs) const;

  int *operator[](int index);

  const int *operator[](int index) const;

  friend std::istream &operator>>(std::istream &is, Matrix &mat);

  friend std::ostream &operator<<(std::ostream &os, const Matrix &mat);
};

#endif