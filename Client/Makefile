CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra 
INCLUDES = -I../libs/boost -I../libs/nanopb -I../generated/proto
LIBS = -lboost_system -lpthread

TARGET = $(BUILD_LOCATION)/client
SRC = $(CURDIR)/client.cpp
.SHELLFLAGS := -lc

all:
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LIBS)

run: all
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
