CXX = g++
CXXFLAGS = -std=c++23 -Wall
LDFLAGS = -ldl -lffi -lreadline -lelf

target: src/* include/*
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o soshell -Iinclude src/*

clean:
	rm -f soshell
