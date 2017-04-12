prologues := 3;
input boxes;
verbatimtex
%&latex
\documentclass[12pt]{article}
\usepackage[T1]{fontenc}
\begin{document}
etex

vardef link(suffix a, b) =
    drawarrow a.c -- b.c cutbefore bpath.a cutafter bpath.b;
enddef;

beginfig(1);
    %%%%%%%%%%%%% fig 1
    circleit.a0(btex $a$ etex);
    circleit.b0(btex $b$ etex);
    circleit.c0(btex $c$ etex);
    circleit.d0(btex $d$ etex);
    circleit.e0(btex $e$ etex);
    circleit.f0(btex $f$ etex);
    circleit.g0(btex $g$ etex);
    circleit.h0(btex $h$ etex);
    circleit.i0(btex $i$ etex);
    circleit.j0(btex $j$ etex);
    circleit.k0(btex $k$ etex);
    circleit.l0(btex $l$ etex);
    circleit.u0(btex $u$ etex);

    numeric diamondx, diamondy, radius;
    a0.c - b0.c = c0.c - d0.c = u0.c - e0.c = f0.c - g0.c = (diamondx, diamondy);
    a0.c - c0.c = b0.c - d0.c = u0.c - f0.c = e0.c - g0.c = (-diamondx, diamondy);
    h0.c - i0.c = (0.8diamondx, diamondy);
    h0.c - j0.c = k0.c - l0.c = (-0.8diamondx, diamondy);
    b0.c - h0.c = (1.8diamondx, diamondy);
    c0.c - k0.c = (-1.8diamondx, diamondy);
    d0.c - u0.c = (-0.5diamondx, diamondy);
    u0.c = origin;
    xpart(a0.e - a0.w) = ypart(a0.n - a0.s) = 2radius;
    xpart(b0.e - b0.w) = ypart(b0.n - b0.s) = 2radius;
    xpart(c0.e - c0.w) = ypart(c0.n - c0.s) = 2radius;
    xpart(d0.e - d0.w) = ypart(d0.n - d0.s) = 2radius;
    xpart(e0.e - e0.w) = ypart(e0.n - e0.s) = 2radius;
    xpart(f0.e - f0.w) = ypart(f0.n - f0.s) = 2radius;
    xpart(g0.e - g0.w) = ypart(g0.n - g0.s) = 2radius;
    xpart(h0.e - h0.w) = ypart(h0.n - h0.s) = 2radius;
    xpart(i0.e - i0.w) = ypart(i0.n - i0.s) = 2radius;
    xpart(j0.e - j0.w) = ypart(j0.n - j0.s) = 2radius;
    xpart(k0.e - k0.w) = ypart(k0.n - k0.s) = 2radius;
    xpart(l0.e - l0.w) = ypart(l0.n - l0.s) = 2radius;
    xpart(u0.e - u0.w) = ypart(u0.n - u0.s) = 2radius;
    xpart(l0.e - i0.w) = 2in;
    ypart(a0.n - g0.s) = 2.5in;
    radius = 0.125in;

    color clr[];
    clr0 = (0.000, 0.584, 0.867); % ancestor shades
    clr1 = (1.000, 0.584, 0.000); % descendant shades
    path p[];
    p0 = fullcircle scaled 3.5radius;
    p1 = (xpart(i0.w), ypart(a0.n))--(xpart(l0.e), ypart(a0.n)); % upper line
    p2 = (xpart(i0.w), ypart(g0.s))--(xpart(l0.e), ypart(g0.s)); % lower line
    p3 = (u0.c){dir 180}..{dir 270}(xpart(e0.w)-.8diamondx,ypart(g0.s)); % descendant left
    p4 = (u0.c){dir   0}..{dir 270}(xpart(f0.e)+.8diamondx,ypart(g0.s)); % descendant right
    p5 = (u0.c){dir 160}..{dir  90}(xpart(b0.w)-  diamondx,ypart(a0.n)); % ancestor left
    p6 = (u0.c){dir  40}..{dir  90}(xpart(c0.e)+  diamondx,ypart(a0.n)); % ancestor left
    fill buildcycle(p0,p5,p1,p6) withcolor clr0;
    fill buildcycle(p0,p3,p2,p4) withcolor clr1;
    draw p0 withpen pencircle scaled 2 withcolor red dashed evenly;
    draw p3 cutbefore p0 withpen pencircle scaled 2 dashed evenly;
    draw p4 cutbefore p0 withpen pencircle scaled 2 dashed evenly;
    draw p5 cutbefore p0 withpen pencircle scaled 2 dashed evenly;
    draw p6 cutbefore p0 withpen pencircle scaled 2 dashed evenly;

    drawboxed(a0, b0, c0, d0, e0, f0, g0, h0, i0, j0, k0, l0, u0);
    link(a0, b0);
    link(a0, c0);
    link(b0, d0);
    link(b0, h0);
    link(c0, d0);
    link(c0, k0);
    link(c0, u0);
    link(d0, j0);
    link(d0, u0);
    link(e0, g0);
    link(f0, g0);
    link(h0, i0);
    link(h0, j0);
    link(k0, f0);
    link(k0, l0);
    link(u0, e0);
    link(u0, f0);

    pair width;
    width = 1.3 (l0.e - i0.w);

    %%%%%%%%%%%%% fig 2
    circleit.a1(btex $a$ etex);
    circleit.b1(btex $b$ etex);
    circleit.c1(btex $c$ etex);
    circleit.d1(btex $d$ etex);
    circleit.h1(btex $h$ etex);
    circleit.i1(btex $i$ etex);
    circleit.j1(btex $j$ etex);
    circleit.k1(btex $k$ etex);
    circleit.l1(btex $l$ etex);

    xpart(a1.e - a1.w) = ypart(a1.n - a1.s) = 2radius;
    xpart(b1.e - b1.w) = ypart(b1.n - b1.s) = 2radius;
    xpart(c1.e - c1.w) = ypart(c1.n - c1.s) = 2radius;
    xpart(d1.e - d1.w) = ypart(d1.n - d1.s) = 2radius;
    xpart(h1.e - h1.w) = ypart(h1.n - h1.s) = 2radius;
    xpart(i1.e - i1.w) = ypart(i1.n - i1.s) = 2radius;
    xpart(j1.e - j1.w) = ypart(j1.n - j1.s) = 2radius;
    xpart(k1.e - k1.w) = ypart(k1.n - k1.s) = 2radius;
    xpart(l1.e - l1.w) = ypart(l1.n - l1.s) = 2radius;

    a1.c - a0.c = width;
    b1.c - b0.c = width;
    c1.c - c0.c = width;
    d1.c - d0.c = width;
    h1.c - h0.c = width;
    i1.c - i0.c = width;
    j1.c - j0.c = width;
    k1.c - k0.c = width;
    l1.c - l0.c = width;

    drawboxed(a1, b1, c1, d1, h1, i1, j1, k1, l1);
    link(a1, b1);
    link(a1, c1);
    link(b1, d1);
    link(b1, h1);
    link(c1, d1);
    link(c1, k1);
    link(d1, j1);
    link(h1, i1);
    link(h1, j1);
    link(k1, l1);

    label(btex \fbox{without $\mathcal{D}^+(u)$} etex, d1.c-(0,2diamondy));

    %%%%%%%%%%%%% fig 3
    circleit.e2(btex $e$ etex);
    circleit.f2(btex $f$ etex);
    circleit.g2(btex $g$ etex);
    circleit.h2(btex $h$ etex);
    circleit.i2(btex $i$ etex);
    circleit.j2(btex $j$ etex);
    circleit.k2(btex $k$ etex);
    circleit.l2(btex $l$ etex);

    xpart(e2.e - e2.w) = ypart(e2.n - e2.s) = 2radius;
    xpart(f2.e - f2.w) = ypart(f2.n - f2.s) = 2radius;
    xpart(g2.e - g2.w) = ypart(g2.n - g2.s) = 2radius;
    xpart(h2.e - h2.w) = ypart(h2.n - h2.s) = 2radius;
    xpart(i2.e - i2.w) = ypart(i2.n - i2.s) = 2radius;
    xpart(j2.e - j2.w) = ypart(j2.n - j2.s) = 2radius;
    xpart(k2.e - k2.w) = ypart(k2.n - k2.s) = 2radius;
    xpart(l2.e - l2.w) = ypart(l2.n - l2.s) = 2radius;

    e2.c - e0.c = 2width;
    f2.c - f0.c = 2width;
    g2.c - g0.c = 2width;
    h2.c - h0.c = 2width;
    i2.c - i0.c = 2width;
    j2.c - j0.c = 2width;
    k2.c - k0.c = 2width;
    l2.c - l0.c = 2width;

    drawboxed(e2, f2, g2, h2, i2, j2, k2, l2);
    link(e2, g2);
    link(f2, g2);
    link(h2, i2);
    link(h2, j2);
    link(k2, f2);
    link(k2, l2);

    label(btex \fbox{without $\mathcal{A}^+(u)$} etex, .5[h2.c,k2.c]+(0,diamondy));

    %%
    z0 = (xpart(i0.w), ypart(a0.n));
    z1 - z0 = z2 - z1 = width;
    label(btex \textbf{\sffamily a.} etex, z0);
    label(btex \textbf{\sffamily b.} etex, z1);
    label(btex \textbf{\sffamily c.} etex, z2);
endfig;
end;
