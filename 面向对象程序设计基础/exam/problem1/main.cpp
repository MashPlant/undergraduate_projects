#include <fstream>
#include "Matrix.h"
using namespace std;
int main()
{
    Matrix a, b, c;

    std::ifstream fin("a.txt");
    fin >> a;
    fin.close();
    fin.open("b.txt");
    fin >> b;
    fin.close();
    fin.open("c.txt");
    fin >> c;

    cout << a + b << endl;
    cout << a * c << endl;
    cout << b * c << endl;

    // Matrix c = a * b;
    // std::ofstream fout("aXb.txt");
    // fout << c;
    // fout.close();
    // fout.open("a+b.txt");
    // c = a + b;
    // fout << c;
    return 0;
}