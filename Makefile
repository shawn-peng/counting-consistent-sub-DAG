#CPPFLAGS+=-O3 -lstdc++
ifeq ($(strip $(OPT)),)
	#default value
	OPT=-g
endif

CPPFLAGS+=$(OPT) -std=c++0x -lstdc++ -lgmpxx -lgmp -lboost_random -lm

all: graph_alg graph_gen analyze_graph batch_graph_gen estimate_bound

graph_alg: main.o graph_alg.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

graph_gen: graph_gen.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

batch_graph_gen: batch_graph_gen.o dag_generator.o dag.o graph_alg.cpp
	$(CC) -o $@ $^ $(CPPFLAGS)

analyze_graph: analyze_graph.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

estimate_bound: estimate_bound.o graph_alg.o dag_generator.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

same_graph_gen: same_graph_gen.o dag_generator.o dag.o graph_alg.cpp
	$(CC) -o $@ $^ $(CPPFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $^ $(CPPFLAGS)

clean:
	rm -f *.o graph_alg


