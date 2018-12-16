CXX = g++
CXXFLAGS = -Wall -std=c++11

program.out: program.o suffix.o
	$(CXX) $(CXXFLAGS) -o program.out program.o suffix.o

program.o: program.cc suffix.h
	$(CXX) $(CXXFLAGS) -c program.cc

suffix.o: suffix.cc suffix.h
	$(CXX) $(CXXFLAGS) -c suffix.cc

clean:
	rm program.out program.o suffix.o