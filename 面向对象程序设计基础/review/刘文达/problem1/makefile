test: main.o member.o
	g++ -std=c++11 -o test main.o member.o
main.o: main.cpp member.o member.h
	g++ -std=c++11 -c -o main.o main.cpp member.o
member.o: member.cpp member.h
	g++ -std=c++11 -c -o member.o member.cpp
