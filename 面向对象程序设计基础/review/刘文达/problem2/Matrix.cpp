#include <iostream>
#include <string>
#include <fstream>
#include "Matrix.h"

Matrix& Matrix::multiply (Matrix &mr) {
    if (column != mr.row) {
        std::cout << "Row != Column.\ndid'nt multiply.\n";
        return *this;
    }
    Matrix tmp (*this);
    column = mr.column;
    for (int i = 0; i < row; ++i) {
        delete[] map[i];
        map[i] = new int[column];
        for (int j = 0; j < column; ++j) {
            for (int k = 0; k < tmp.column; ++k)
                map[i][j] += tmp.map[i][k] * mr.map[k][j];
        }
    }
    return *this;
}

void Matrix::load (std::string input) {
    std::fstream file (input);
    file >> row >> column;
    map = new int *[row];
    for (int i = 0; i < row; ++i) {
        map[i] = new int[column];
        for (int j = 0; j < column; ++j) {
            file >> map[i][j];
        }
    }
    file.close ();
}

void Matrix::display () const {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            std::cout << map[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

void Matrix::save (std::string output) const {
    std::ofstream fout (output);
    fout << row << ' ' << column << '\n';
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            fout << map[i][j] << ' ';
        }
        fout << '\n';
    }
    fout.close ();
}

Matrix::Matrix (std::string input) {
    load (std::move (input));
}

Matrix::Matrix (const Matrix &a) {
    row = a.row, column = a.column;
    map = new int *[row];
    for (int i = 0; i < row; ++i) {
        map[i] = new int[column];
        for (int j = 0; j < column; ++j) {
            map[i][j] = a.map[i][j];
        }
    }
}

Matrix::~Matrix () {
    for (int i = 0; i < row; ++i) delete[] map[i];
    delete[] map;
}