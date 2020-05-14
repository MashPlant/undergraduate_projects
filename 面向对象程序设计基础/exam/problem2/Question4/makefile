SOURCES=$(wildcard *.cpp)
HEADERS=$(wildcard *.h)
OBJECTS=$(SOURCES:%.cpp=%.o)
TARGET=test.exe

all: $(TARGET)
$(TARGET): $(OBJECTS) $(HEADERS)
	@echo "Now Generating $(TARGET) ..."
	g++ $(OBJECTS) $(LIB) -std=c++11 -o $(TARGET) -O2
%.o: %.cpp $(HEADERS)
	@echo "Now Compiling $< ..."
	g++ -I. -c -std=c++11 $< -o $@ -O2
clean:
	del *.o *.exe *.bak
explain:
	@echo "Headers: $(HEADERS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Target: $(TARGET)"
