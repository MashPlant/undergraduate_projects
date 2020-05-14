problem1.exe : polynomial.o posynomial.o main.o
	g++ -std=c++11 polynomial.o posynomial.o main.o -o problem1.exe
main.o : main.cpp
	g++ -std=c++11 -c main.cpp -o main.o
polynomial.o : polynomial.cpp
	g++ -std=c++11 -c polynomial.cpp -o polynomial.o
posynomial.o : posynomial.cpp
	g++ -std=c++11 -c posynomial.cpp -o posynomial.o
clean : 
	del *.o *.exe