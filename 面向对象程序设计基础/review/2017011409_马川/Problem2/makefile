sSOURCES=$(wildcard *.cpp)
HEADERS=$(wildcard *.h)

sOBJECTS=$(sSOURCES:%.cpp=%.o)

TARGET=cmpRuntime.exe

all: $(TARGET)
$(TARGET): $(sOBJECTS) $(HEADERS) 
	@echo "Now Generating $(TARGET) ..."
	g++ $(sOBJECTS) -o $(TARGET) -O2 

%.o: %.cpp $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -c $< -o $@ -O2 

clean:
	rm *.o *.exe 
explain:
	@echo "User Sources: $(sSOURCES)"
	@echo "User Objects: $(sOBJECTS)"
	@echo "Target: $(TARGET)"
