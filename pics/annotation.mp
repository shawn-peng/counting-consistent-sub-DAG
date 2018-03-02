prologues := 3;
input metaobj;
verbatimtex
%&latex
\documentclass[4pt]{article}
\usepackage[T1]{fontenc}
\begin{document}
etex

vardef link(suffix a, b) =
    drawarrow a.c -- b.c cutbefore bpath.a cutafter bpath.b;
enddef;

beginfig(1);
    newCircle.a("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.b("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.c("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.d("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.e("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.f("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.g("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.h("") "circmargin(0.125in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";

    numeric diamondx, diamondy, radius;
    a.c = origin;
    a.c - b.c = b.c - d.c = c.c - e.c = f.c - g.c = (diamondx, diamondy);
    a.c - c.c = c.c - f.c = (-diamondx, diamondy);
    g.c - h.c = (0, diamondy);
    % xpart(a.e - a.w) = ypart(a.n - a.s) = 2radius;
    % xpart(b.e - b.w) = ypart(b.n - b.s) = 2radius;
    % xpart(c.e - c.w) = ypart(c.n - c.s) = 2radius;
    % xpart(d.e - d.w) = ypart(d.n - d.s) = 2radius;
    % xpart(e.e - e.w) = ypart(e.n - e.s) = 2radius;
    % xpart(f.e - f.w) = ypart(f.n - f.s) = 2radius;
    % xpart(g.e - g.w) = ypart(g.n - g.s) = 2radius;
    % xpart(h.e - h.w) = ypart(h.n - h.s) = 2radius;
    xpart(f.e - d.w) = 1.5in;
    ypart(a.n - h.s) = 2.5in;
    radius = 0.125in;

    color clr[];
    clr0 = (0.000, 0.584, 0.867); % blue: ancestor shades
    clr1 = (1.000, 0.584, 0.000); % yellow: descendant shades

    % pickup pencircle scaled 1.25;
    % drawboxed(a, b, c, d, e, f, g, h);
    % pickup pencircle scaled 1;
    drawObj(a);
    drawObj(b);
    drawObj(c);
    drawObj(d);
    drawObj(e);
    drawObj(f);
    drawObj(g);
    drawObj(h);
    ncline(a)(b);
    ncline(a)(c);
    ncline(b)(d);
    ncline(c)(e);
    ncline(c)(f);
    ncline(e)(g);
    ncline(f)(g);
    ncline(g)(h);

    label.rt(btex molecular function etex, a.e);
    label.lft(btex \begin{tabular}{@{}c@{}}nucleic acid binding\\TF activity\end{tabular} etex, b.w);
    label.rt(btex binding etex, c.e);
    label.lft(btex \begin{tabular}{@{}c@{}}sequence-specific DNA\\binding TF activity\end{tabular} etex, d.w);
    label.llft(btex \begin{tabular}{@{}c@{}}organic cyclic\\compound binding\end{tabular} etex, (e.s + .75(a.e-a.w)));
    label.rt(btex \begin{tabular}{@{}c@{}}heterocyclic compound\\binding\end{tabular} etex, f.e);
    label.rt(btex nucleic acid binding etex, g.e);
    label.rt(btex DNA binding etex, h.e);

    % pair width;
    % width = 1.3 (l0.e - i0.w);

    % %%
    % z0 = (xpart(i0.w), ypart(a0.n));
    % z1 - z0 = z2 - z1 = width;
    % label(btex \textbf{\sffamily a.} etex, z0);
    % label(btex \textbf{\sffamily b.} etex, z1);
    % label(btex \textbf{\sffamily c.} etex, z2);
endfig;
end;
