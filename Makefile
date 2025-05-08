# Makefile

CXX = g++
CXXFLAGS = -O2 -Wall -pthread
TARGET = main
SRC = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
