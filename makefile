CXX = g++
CXXFLAGS = -Wall -std=c++11

ifeq ($(OS),Windows_NT)
    PROGRAM_OUT := program.exe
	RM := del
else
    PROGRAM_OUT := program.out
	RM := rm
endif

$(PROGRAM_OUT): program.o suffix.o
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_OUT) program.o suffix.o

program.o: program.cc suffix.h
	$(CXX) $(CXXFLAGS) -c program.cc

suffix.o: suffix.cc suffix.h
	$(CXX) $(CXXFLAGS) -c suffix.cc

clean:
	$(RM) program.out program.o suffix.o