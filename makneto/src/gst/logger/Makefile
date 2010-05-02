
all: logger

CXXFLAGS=-Wall -g -ansi

logger: logger.h logger.o test.o
	$(CXX) $(CXXFLAGS) -o logger logger.o test.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
