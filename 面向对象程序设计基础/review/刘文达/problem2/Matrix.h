#ifndef LYRA_MATRIX_H
#define LYRA_MATRIX_H

#include <iostream>
#include <string>

class Matrix {
    int row,column;
    int** map;
public:
    Matrix () {
        row = column = 0;
        map = nullptr;
    }
    Matrix (const Matrix& a);
    Matrix (std::string);
    void load (std::string);
    void save (std::string) const;
    void display () const;
    Matrix& multiply (Matrix&);
    ~Matrix ();
};

#endif //LYRA_MATRIX_H
