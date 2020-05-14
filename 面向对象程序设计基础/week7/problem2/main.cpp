#include "Matrix.h"

int main()
{
    Matrix a("a.txt"), b;     //a.txt stores a matrix of 6 rows and 8 columns
    b.load("b.txt");          //b.txt stores a matrix of 8 rows and 6 columns
    a.display();              //print matrix a
    b.display();              //print matrix b
    Matrix c = a.multiply(b); //compute the product
    c.display();              //print the product
    c.save("aXb.txt");        //get a matrix of 6 rows and 6 columns
    return 0;
}