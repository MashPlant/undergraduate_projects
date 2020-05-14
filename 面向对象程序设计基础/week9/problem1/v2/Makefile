main.exe : main.o OptRouter.o maze.a
	g++ -std=c++11 $^ -o $@

main.o : main.cpp BaseRouter.h OptRouter.h
	g++ -c -std=c++11 $< -o $@

OptRouter.o : OptRouter.cpp BaseRouter.h OptRouter.h
	g++ -c -std=c++11  $< -o $@

.PHONY : clean

clean : 
	rm *.o *.exe 
