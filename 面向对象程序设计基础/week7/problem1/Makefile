main.exe : main.o member.o member.h 
	g++ main.o member.o -o main.exe
main.o: main.cpp
	g++ -c main.cpp -o main.o
member.o : member.cpp member.h
	g++ -c member.cpp -o member.o

.PHONY : clean
clean :
	del *.o *.exe