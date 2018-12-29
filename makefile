CXX = g++
CXXFLAGS = -Wall -std=c++11 -g

SAIS_LCP_FILE_NAME = sais_lcp
SAIS_LCP_MAIN_NAME_EXTENSION = _main
SAIS_LCP_MAIN_FILE_NAME := $(SAIS_LCP_FILE_NAME)$(SAIS_LCP_MAIN_NAME_EXTENSION)

ifeq ($(OS),Windows_NT)
    PROGRAM_OUT := program.exe
	SAIS_LCP_MAIN_OUT := $(SAIS_LCP_MAIN_FILE_NAME).exe
	RM := del
else
    PROGRAM_OUT := program.out
	SAIS_LCP_MAIN_OUT := $(SAIS_LCP_MAIN_FILE_NAME).out
	RM := rm
endif

all: $(PROGRAM_OUT) $(SAIS_LCP_MAIN_OUT)

$(PROGRAM_OUT): program.o suffix.o
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_OUT) program.o suffix.o

program.o: program.cc suffix.h
	$(CXX) $(CXXFLAGS) -c program.cc

suffix.o: suffix.cc suffix.h
	$(CXX) $(CXXFLAGS) -c suffix.cc

$(SAIS_LCP_MAIN_OUT): $(SAIS_LCP_MAIN_FILE_NAME).o $(SAIS_LCP_FILE_NAME).o
	$(CXX) $(CXXFLAGS) -o $(SAIS_LCP_MAIN_OUT) $(SAIS_LCP_MAIN_FILE_NAME).o $(SAIS_LCP_FILE_NAME).o

$(SAIS_LCP_MAIN_FILE_NAME).o: $(SAIS_LCP_MAIN_FILE_NAME).cc $(SAIS_LCP_FILE_NAME).h
	$(CXX) $(CXXFLAGS) -c $(SAIS_LCP_MAIN_FILE_NAME).cc

$(SAIS_LCP_FILE_NAME).o: $(SAIS_LCP_FILE_NAME).cc $(SAIS_LCP_FILE_NAME).h
	$(CXX) $(CXXFLAGS) -c $(SAIS_LCP_FILE_NAME).cc

# $(SAIS_LCP_MAIN_OUT): sais_lcp_main.o sais_lcp.o
# 	$(CXX) $(CXXFLAGS) -o $(SAIS_LCP_MAIN_OUT) sais_lcp_main.o sais_lcp.o

# sais_lcp_main.o: sais_lcp_main.cc sais_lcp.h
# 	$(CXX) $(CXXFLAGS) -c sais_lcp_main.cc

# sais_lcp.o: sais_lcp.cc sais_lcp.h
# 	$(CXX) $(CXXFLAGS) -c sais_lcp.cc

clean:
	$(RM) program.out program.o suffix.o