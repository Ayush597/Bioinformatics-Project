CXX = g++
CXXFLAGS = -Wall -std=c++11 -g -O3

all: stud_main.exe

stud_main.exe: stud_sais_lcp_original.o stud_main.o stud_sais.o
	$(CXX) $(CXXFLAGS) -o $@ $^

stud_sais.o: stud_sais.cc stud_sais.h
	$(CXX) $(CXXFLAGS) -c $*.cc

stud_sais_lcp_original.o: stud_sais_lcp_original.cc stud_sais_lcp_original.h
	$(CXX) $(CXXFLAGS) -c $*.cc

clean:
	$(RM) -f stud_main.exe stud_sais.o stud_main.o stud_sais_lcp_original.o
