CXX = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = client
SRC = client.cpp
LIBS = -lboost_system

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
