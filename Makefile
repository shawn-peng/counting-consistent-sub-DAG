#CPPFLAGS+=-O3 -lstdc++
ifeq ($(strip $(OPT)),)
	#default value
	OPT=-g
endif

CPPFLAGS+=$(OPT) -std=c++0x -lstdc++ -lgmpxx -lgmp -lboost_random

all: graph_alg graph_gen analyze_graph batch_graph_gen

graph_alg: main.o graph_alg.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

graph_gen: graph_gen.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

batch_graph_gen: batch_graph_gen.o dag_generator.o dag.o graph_alg.cpp
	$(CC) -o $@ $^ $(CPPFLAGS)

analyze_graph: analyze_graph.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $^ $(CPPFLAGS)

clean:
	rm -f *.o graph_alg


