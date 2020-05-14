CC = g++
CPP_FLAGS = -std=c++17 -O2 -w

main.exe : main.o BaseTest.o 
	$(CC) $(CPP_FLAGS) main.o BaseTest.o -o main.exe

main.o : main.cpp *.h ../src/*.h
	$(CC) -c $(CPP_FLAGS) main.cpp -o main.o 

BaseTest.o : BaseTest.cpp BaseTest.h ../src/*.h 
	$(CC) -c $(CPP_FLAGS) BaseTest.cpp -o BaseTest.o 

.PHONY : clean, profile

profile:
	$(CC) $(CPP_FLAGS) -g prof.cpp -o prof.exe	
	chmod 700 prof.exe
	valgrind --tool=callgrind ./prof.exe -o callgrind.out
	kcachegrind callgrind.out
	
clean : 
	rm *.exe *.o
