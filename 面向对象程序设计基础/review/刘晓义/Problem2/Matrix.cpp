#include "Matrix.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

int* Matrix::operator[](size_t pos) {
  // The mutable version
  return &this->content[pos * this->width];
}

Matrix::Matrix():content(nullptr) {}

Matrix::Matrix(size_t h, size_t w)
  :height(h), width(w), content(new int[h*w]) {}

Matrix::Matrix(const char *fn):content(nullptr) {
  this->load(fn);
}

void Matrix::load(const char *fn) {
  ifstream fis(fn);
  if(fis.fail()) throw invalid_argument(string("Unable to open file: ") + fn);
  fis>>this->height>>this->width;

  // Reallocate the content, deallocate the previous if any
  this->content.reset(new int[this->height * this->width]);

  for(size_t i = 0; i<this->height * this->width; ++i)
    fis>>this->content[i];
}

void Matrix::display() const {
  cout<<*this;
}

void Matrix::display_product() const {
  this->display();
}

void Matrix::save_product(const char *fn) const {
  ofstream fos(fn);
  fos<<*this;
}

const int* Matrix::operator[](size_t pos) const {
  // The immutable version
  return &this->content[pos * this->width];
}

Matrix Matrix::multiply(const Matrix &ano) const {
  if(this->width != ano.height)
    throw invalid_argument("Dimension mismatch");

  Matrix result(this->height, ano.width);

  for(size_t i = 0; i < this->height; ++i) {
    for(size_t j = 0; j < ano.width; ++j) {
      result[i][j] = 0;

      for(size_t k = 0; k < this->width; ++k)
        result[i][j] += (*this)[i][k] * ano[k][j];
    }
  }

  return result;
}
