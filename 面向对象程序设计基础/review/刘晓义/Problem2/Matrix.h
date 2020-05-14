#ifndef __MATRIX_H_
#define __MATRIX_H_

#include <memory>
#include <ostream>

class Matrix {
  /**
   * The height of the matrix
   */
  size_t height = -1;

  /**
   * The width of the matrix
   */
  size_t width = -1;

  /**
   * Smart pointer pointing to the content of the matrix
   */
  std::unique_ptr<int[]> content;

  /**
   * Helper: subscript operator proxied to the content, mutable
   */
  int* operator[](size_t pos);

public:
  /**
   * Create an uninitialized matrix
   */
  Matrix();

  /**
   * Create a matrix with known dimensions and allocate needed space for it
   * @args h: the height
   * @args w: the width
   */
  Matrix(size_t h, size_t w);

  /**
   * Create a matrix with the content of a file
   * @args fn: filename
   */
  Matrix(const char *fn);

  /**
   * Manually load the content of a file into this instance
   * @args fn: filename
   * @thows invalid_argument: When the file cannot be accessed
   */
  void load(const char *fn);

  /**
   * Return the result of multiplication between this instance an another
   * @args ano: another matrix
   * @thows invalid_argument: When we cannot multiply the two matrix
   */
  Matrix multiply(const Matrix &ano) const;

  /**
   * Print the content of the matrix to the console
   */
  void display() const;

  /**
   * Print the content of the matrix to the console
   * Identical as display, for API compatibility 
   */
  void display_product() const;

  /**
   * Save the content of the matrix into a file
   * @args fn: filename
   */
  void save_product(const char *fn) const;

  /**
   * Helper: subscript operator proxied to the content, immutable
   * @args pos: the subscript
   */
  const int* operator[](size_t pos) const;

  /**
   * Streaming the content of this matrix.
   */
  template<typename T>
  friend T& operator<<(T& s, const Matrix &m) {
    s<<m.height<<" "<<m.width<<std::endl;
    for(size_t i = 0; i<m.height; ++i) {
      for(size_t j = 0; j<m.width; ++j)
        s<<m[i][j]<<" ";
      s<<std::endl;
    }

    return s;
  }
};

#endif // __MATRIX_H_
