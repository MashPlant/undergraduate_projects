main: main.o Matrix.o *.h
	g++ -o main main.o Matrix.o -g -Wall -Wextra -Werror

main.o: main.cpp *.h
	g++ -c -o main.o main.cpp -std=c++11 -Wall -g -Wextra -Werror

Matrix.o: Matrix.cpp *.h
	g++ -c -o Matrix.o Matrix.cpp -std=c++11 -Wall -g -Wextra -Werror

clean:
	rm -rf *.o main

.PHONY: clean
