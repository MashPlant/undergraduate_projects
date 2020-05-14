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

  //这种语法并非是右值引用,而是既可以绑定左值也可以绑定右值的引用
  //它可以称作Forwarding references,详见 http://en.cppreference.com/w/cpp/language/reference
  template <typename OS>
  void output(OS &&os) const;

public:
  Matrix() = default;

  //生成r行c列的矩阵(元素全置为0)
  Matrix(int r, int c);

  //禁止隐式类型转换
  explicit Matrix(const char *file);

  void load(const char *file);

  // rhs : right hand side
  Matrix multiply(const Matrix &rhs) const;

  void display() const;

  void save(const char *file) const;
};

#endif