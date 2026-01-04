CXX = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = server
SRC = server.cpp
LIBS = -lboost_system

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
