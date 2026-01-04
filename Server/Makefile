CXX = g++
CXXFLAGS = -std=c++17 -I./libs/boost
SRC = $(CURDIR)/server.cpp
LIBS = -lboost_system -lpthread -Llibs/boost/stage/lib -lws2_32

TARGET = $(BUILD_LOCATION)/server
.SHELLFLAGS := -lc

all:
	mkdir -p $(TARGET)
	@$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: all
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
