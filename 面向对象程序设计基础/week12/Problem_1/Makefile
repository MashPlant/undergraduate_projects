HEADERS = $(wildcard *.h)

main.exe : $(HEADERS) main.o KMP.a
	g++ main.o KMP.a -o main.exe -std=c++11 

main.o : $(HEADERS) main.cpp
	g++ -c main.cpp -o main.o -std=c++11

KMP.a : KMP.h KMP.cpp
	g++ -c KMP.cpp -o KMP.o -std=c++11
	ar -rv KMP.a KMP.o
	ranlib KMP.a 

.PHONY : clean

clean :
	rm *.o *.a *.exe

