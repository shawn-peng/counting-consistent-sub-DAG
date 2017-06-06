CXX = g++
CFLAGS += -std=c++11
CFLAGS += -Wall
CFLAGS += -O3

HEADERS = util.h
OBJS = util.o cdag.o
LIBS =

all: cdag simdag

cdag: util.o cdag.o
	$(CXX) $(CFLAGS) -o cdag $(OBJS) $(LIBS)

simdag: simdag.cpp
	$(CXX) $(CFLAGS) -o simdag simdag.cpp

util.o: util.cpp $(HEADERS)
	$(CXX) -c $(CFLAGS) util.cpp

cdag.o: cdag.cpp $(HEADERS)
	$(CXX) -c $(CFLAGS) cdag.cpp

clean:
	rm $(OBJS) cdag simdag
