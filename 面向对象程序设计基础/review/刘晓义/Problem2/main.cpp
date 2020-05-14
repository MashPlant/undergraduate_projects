// main.cpp

#include "Matrix.h"

int main() {
  Matrix a("a.txt"), b;
  b.load("b.txt");

  a.display();
  b.display();

  Matrix c = a.multiply(b);

  c.display_product();
  c.save_product("aXb.txt");

  return 0;
}
