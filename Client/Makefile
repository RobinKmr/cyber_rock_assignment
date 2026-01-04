CXX = g++
CXXFLAGS = -std=c++17 -Wall -I../libs/boost
SRC = $(CURDIR)/client.cpp
INCLUDES = -I../libs
LIBS = -lboost_system -lpthread

TARGET = $(BUILD_LOCATION)/client
.SHELLFLAGS := -lc

all:
	@$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(INCLUDES) $(LIBS)

run: all
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
