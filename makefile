CXX = g++
CXXFLAGS = -Wall -std=c++11 -g
SRC = scanline.cpp
TARGET = $(SRC:.cpp=)
LDFLAGS = -ldl -lglfw

all:build

build:
	$(CXX) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)