function draw_dag(datafile)

O = tdfread(datafile);

G = digraph();
G = G.addedge(O.source,O.target);

G.plot();

end