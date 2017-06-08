all: algorithm.pdf prune.pdf

algorithm.pdf: algorithm.mp
	mpost algorithm.mp
	mv algorithm.1 algorithm.eps
	epstopdf algorithm.eps

prune.pdf: prune.mp
	mpost prune.mp
	mv prune.1 prune.eps
	epstopdf prune.eps

clean:
	rm *.mpx *.log *.eps *.pdf
