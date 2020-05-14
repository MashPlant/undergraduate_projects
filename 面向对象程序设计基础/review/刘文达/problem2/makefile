test: main.o matrix.o Matrix.h
	g++ -std=c++11 -fsanitize=address -o test main.o matrix.o
main.o: main.cpp Matrix.h matrix.o
	g++ -std=c++11 -fsanitize=address -c -o main.o main.cpp matrix.o
matrix.o: Matrix.cpp Matrix.h
	g++ -std=c++11 -fsanitize=address -c -o matrix.o Matrix.cpp
