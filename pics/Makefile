all: algorithm.pdf prune.pdf example.pdf

algorithm.pdf: algorithm.mp
	mpost algorithm.mp
	mv algorithm.1 algorithm.eps
	epstopdf algorithm.eps

prune.pdf: prune.mp
	mpost prune.mp
	mv prune.1 prune.eps
	epstopdf prune.eps

example.pdf: example.mp
	mpost example.mp
	mv example.1 example.eps
	epstopdf example.eps

clean:
	rm *.mpx *.log *.eps *.pdf
