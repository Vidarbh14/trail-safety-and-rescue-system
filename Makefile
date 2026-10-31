CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude
TARGET = main.exe
SRCS = main.cpp src/TrailGraph.cpp src/TrailSafetySystem.cpp src/UI.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
