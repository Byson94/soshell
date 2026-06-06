CXX = g++
CXXFLAGS = -std=c++23 -Wall
LDFLAGS = -ldl

target: src/main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o soshell src/main.cpp

clean:
	rm -f soshell
