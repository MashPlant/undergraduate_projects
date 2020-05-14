command = -std=c++11 -O3 
main.exe : main.o Matrix.o Matrix.h
	g++ $(command) main.o Matrix.o -o main.exe
main.o : main.cpp
	g++ -c $(command) main.cpp -o main.o 
Matrix.o : Matrix.h Matrix.cpp
	g++ -c $(command) Matrix.cpp -o Matrix.o

.PHONY : clean
clean :
	del *.o *.exe
