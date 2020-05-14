main.exe : main.o Graph.o MST.o 
	g++ -std=c++11 main.o Graph.o MST.o -o main.exe
main.o : main.cpp 
	g++ -std=c++11 -c main.cpp -o main.o
Graph.o : Graph.h Graph.cpp
	g++ -std=c++11 -c Graph.cpp -o Graph.o
MST.o : MST.h MST.cpp UFS.h
	g++ -std=c++11 -c MST.cpp -o MST.o
clear :
	del *.o *.exe