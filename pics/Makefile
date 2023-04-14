all: pivot.pdf prune.pdf example.pdf annotation.pdf

pivot.pdf: pivot.mp
	mpost pivot.mp
	mv pivot.1 pivot.eps
	epstopdf pivot.eps

prune.pdf: prune.mp
	mpost prune.mp
	mv prune.1 prune.eps
	epstopdf prune.eps

example.pdf: example.mp
	mpost example.mp
	mv example.1 example.eps
	epstopdf example.eps

annotation.pdf: annotation.mp
	mpost annotation.mp
	mv annotation.1 annotation.eps
	epstopdf annotation.eps

clean:
	rm *.mpx *.log *.eps *.pdf
