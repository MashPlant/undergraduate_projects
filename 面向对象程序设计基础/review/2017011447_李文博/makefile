test.exe: graph.o mst.o main.o graph.h mst.h head.h
	g++ graph.o mst.o main.o -o test.exe
graph.o: graph.cpp graph.h
	g++ -c graph.cpp -o graph.o
mst.o: mst.cpp mst.h
	g++ -c mst.cpp -o mst.o
main.o: main.cpp graph.cpp mst.cpp
	g++ -c main.cpp -o main.o
.PHONY: clean
clean:
	del *.o *.exe