#CPPFLAGS+=-O3 -lstdc++
CPPFLAGS+=$(OPT) -lstdc++

all: graph_alg

graph_alg: graph_alg.o dag.o
	$(CC) -o $@ $^ $(CPPFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $^ $(CPPFLAGS)

clean:
	rm -f *.o graph_alg


