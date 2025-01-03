CXX = g++
CXXFLAGS = -Wall -std=c++11 -g
GLAD = glad.c
TARGET = scanline
SRC = scanline.cpp
OBJ = $(SRC:.cpp=.o)
LIBS = -ldl -lglfw

# Compilation rule
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GLAD) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLFLAGS) -c $< -o $@

# Clean up object files and executables
clean:
	rm -f $(OBJ) $(TARGET)

# Run the executable (optional)
run: $(TARGET)
	./$(TARGET)