#CPPFLAGS+=-O3 -lstdc++
ifeq ($(strip $(OPT)),)
	#default value
	OPT=-g
endif

CPPFLAGS+=$(OPT) -std=c++0x -lstdc++ -lgmpxx -lgmp

all: graph_alg

#alg_test

graph_alg: main.o graph_alg.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

alg_test: test.o graph_alg.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $^ $(CPPFLAGS)

clean:
	rm -f *.o graph_alg


