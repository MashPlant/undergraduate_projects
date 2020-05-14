HEADERS = $(wildcard *.h)
CPP_FLAGS = -std=c++11 -O2

main.exe : main.o KMP.o $(HEARDERS)
	g++ main.o KMP.o -o main.exe $(CPP_FLAGS)

main.o : $(HEADERS) main.cpp	
	g++ -c main.cpp -o main.o $(CPP_FLAGS)

KMP.o : KMP.h KMP.cpp
	g++ -c KMP.cpp -o KMP.o $(CPP_FLAGS)

clean :
	rm *.o *.exe *.txt
