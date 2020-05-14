#lSOURCES=$(wildcard *.cxx)
sSOURCES=$(wildcard *.cpp)
HEADERS=$(wildcard *.h)
#lOBJECTS=$(lSOURCES:%.cxx=%.o)
sOBJECTS=$(sSOURCES:%.cpp=%.o)
TARGET=match.exe
LIB=KMP.a

all: $(TARGET) $(LIB)
$(TARGET):$(HEADERS) $(LIB)
	@echo "Now Generating $(TARGET) ..."
	g++ $(LIB) -o $(TARGET) -fsanitize=address
$(LIB): $(sOBJECTS) $(HEADERS)
	@echo "Now Generating $(LIB) ..."
	ar -rv $(LIB) $(sOBJECTS)
	ranlib $(LIB)
%.o: %.cpp $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -I. -c $< -o $@ -fsanitize=address
#%.o: %.cxx $(HEADERS)
#	@echo "Now Compiling $< ..."
#	g++ -I. -c $< -o $@
clean:
	rm *.o *.exe 
upload:
	del *.o
explain:
	@echo "Headers: $(HEADERS)"
#	@echo "Lib Sources: $(lSOURCES)"
#	@echo "Lib Objects: $(lOBJECTS)"
	@echo "User Sources: $(sSOURCES)"
	@echo "User Objects: $(sOBJECTS)"
	@echo "Lib: $(LIB)"
	@echo "Target: $(TARGET)"

