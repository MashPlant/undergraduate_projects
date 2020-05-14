main: main.o member.o *.h
	g++ -o main main.o member.o -g -O3 -Wall -Wextra -Werror

main.o: main.cpp *.h
	g++ -c -o main.o main.cpp -std=c++11 -Wall -g -O3 -Wextra -Werror

member.o: member.cpp *.h
	g++ -c -o member.o member.cpp -std=c++11 -Wall -g -O3 -Wextra -Werror

clean:
	rm -rf *.o main

.PHONY: clean
