CXX = g++
CXXFLAGS = -Wall
INCLUDES = -I../libs/boost -I../libs/nanopb -I../generated/proto
LIBS = -lboost_system

TARGET = server
SRC = $(CURDIR)/server.cpp \
	../libs/nanopb/pb_encode.h \
	../libs/nanopb/pb_decode.h \
	../libs/nanopb/pb.h \
	../generated/proto/sensor.pb.h

all:
# 	@mkdir -p $(BUILD_LOCATION)/server
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) $(LIBS)  -fexceptions -g -std=c++17 -o $(TARGET)

run: all
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
