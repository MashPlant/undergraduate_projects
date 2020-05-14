objects = main.o graph.o mst.o mst_Prim.o mst_Kruskal.o

graph.exe: $(objects) graph.h mst.h
	g++ $(objects) -o $@

main.o: graph.h mst.h
graph.o: graph.h
mst.o: mst.h graph.h
mst_Prim.o: mst.h graph.h
mst_Kruskal.o: mst.h graph.h

.PHONY: clean
clean: 
	del *.o *.exe *.txt