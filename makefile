CXX = g++
CXXFLAGS = -std=c++11 -O3

SAIS_LCP_FILE_NAME = sais_lcp
SAIS_LCP_MAIN_NAME_EXTENSION = _main
SAIS_LCP_MAIN_FILE_NAME = $(SAIS_LCP_FILE_NAME)$(SAIS_LCP_MAIN_NAME_EXTENSION)
TEST_FILE_NAME = test

ifeq ($(OS),Windows_NT)
	SAIS_LCP_MAIN_OUT := $(SAIS_LCP_MAIN_FILE_NAME).exe
	TEST_OUT := $(TEST_FILE_NAME).exe
	RM := del
else
	SAIS_LCP_MAIN_OUT := $(SAIS_LCP_MAIN_FILE_NAME).out
	TEST_OUT := $(TEST_FILE_NAME).out
	RM := rm
endif

all: $(SAIS_LCP_MAIN_OUT) $(TEST_OUT)

# $@: the target filename.
# $*: the target filename without the file extension.
# $<: the first prerequisite filename.
# $^: the filenames of all the prerequisites, separated by spaces, discard duplicates.
# $+: similar to $^, but includes duplicates.
# $?: the names of all prerequisites that are newer than the target, separated by spaces.

$(SAIS_LCP_MAIN_OUT): sais_lcp_main.o sais_lcp.o sa_to_lcp.o sais_util.o sais.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SAIS_LCP_MAIN_FILE_NAME).o: sais_lcp_main.cc sais_lcp.h sa_to_lcp.h sais_util.h
	$(CXX) $(CXXFLAGS) -c $*.cc

$(TEST_OUT): test.o sais_lcp.o sa_to_lcp.o sais_util.o sais.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_FILE_NAME).o: test.cc sais_lcp.h sa_to_lcp.h sais_util.h
	$(CXX) $(CXXFLAGS) -c $*.cc

sais_lcp.o: sais_lcp.cc sais_lcp.h sais_util.h sa_to_lcp.h
	$(CXX) $(CXXFLAGS) -c $*.cc

sais_util.o: sais_util.cc sais_util.h
	$(CXX) $(CXXFLAGS) -c $*.cc

sa_to_lcp.o: sa_to_lcp.cc sa_to_lcp.h sais.h
	$(CXX) $(CXXFLAGS) -c $*.cc

sais.o: sais.c sais.h
	g++ -std=c++11 -c sais.c

clean:
	$(RM) -f $(SAIS_LCP_MAIN_OUT) $(TEST_OUT) \
			sais_lcp_main.o test.o sais_lcp.o sais_util.o sa_to_lcp.o sais.o
