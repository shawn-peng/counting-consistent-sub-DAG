prologues := 3;
input metaobj;
verbatimtex
%&latex
\documentclass[12pt]{article}
\usepackage[T1]{fontenc}
\begin{document}
etex

vardef drawzzarrow(suffix a,b) =
    save v;
    newCircle.v("") "filled(true)","circmargin(0.05in)";
    numeric r[];
    r1 = xpart(a.e - a.w)/2;
    r2 = xpart(v.e - v.w)/2;
    pair x;
    x = (a.c--b.c) intersectionpoint BpathCircle(a);
    a.c - x = (v.c - x) * (r1/r2);
    drawObj(v);
    nczigzag(a)(b), "linetension(0.8)", "coilarmA(0mm)", "coilarmB(2mm)", "coilwidth(2mm)", "linearc(.1mm)";
enddef;

beginfig(1);
    pair anchor[];
    anchor1 = origin;
    anchor2 - anchor1 = (1.8in, 0);
    anchor3 - anchor2 = (2in, 0);

    label(btex \textbf{\sffamily a.} etex, anchor1);
    % label(btex \textbf{\sffamily b.} etex, anchor2);
    label(btex \textbf{\sffamily b.} etex, anchor3);

    %%%%%%%%%%%
    %% figure 1
    %%%%%%%%%%%
    unit1 := 0.6in;
    newCircle.u1("") "circmargin(0.05in)";
    newCircle.u2("") "circmargin(0.05in)";
    newCircle.u3("") "circmargin(0.05in)";
    newCircle.u4("") "circmargin(0.05in)";
    newCircle.u5("") "circmargin(0.05in)";
    newCircle.u6("") "circmargin(0.05in)";
    newCircle.u7("") "circmargin(0.05in)";
    u1.c = anchor1 shifted (1.2unit1, -0.2unit1);
    u2.c - u1.c = u4.c - u3.c = (-0.5unit1, -0.4unit1);
    u4.c - u5.c = (0.1unit1, 0.6unit1);
    u3.c - u1.c = (0.6unit1, -0.5unit1);
    u5.c - u6.c = (-0.4unit1, 0.8unit1);
    u5.c - u7.c = (0.4unit1, 0.8unit1);
    for i = 1 upto 7: drawObj(u[i]); endfor
    ncline(u1)(u2);
    ncline(u1)(u3);
    ncline(u2)(u4);
    ncline(u2)(u5);
    ncline(u3)(u4);
    ncline(u4)(u5);
    ncline(u5)(u6);
    ncline(u5)(u7);

    label(btex $u$ etex, u5.c shifted (0.25unit1,-0.18unit1));

    %%%%%%%%%%%
    %% figure 2
    %%%%%%%%%%%
    unit2 = unit1;
    newCircle.ostem(btex \rule{0in}{0.45in} etex) "circmargin(0.3in)","framestyle(dashed evenly)";
    newEllipse.D(btex \rule{0.3in}{0in} etex) "fit(true)","circmargin(0.2in)","framestyle(dashed evenly)";

    newCircle.v1("") "circmargin(0.05in)";
    newCircle.v2("") "circmargin(0.05in)";
    newCircle.v3("") "circmargin(0.05in)";
    newCircle.v4("") "circmargin(0.05in)";
    newCircle.v5("") "circmargin(0.05in)";
    newCircle.v6("") "circmargin(0.05in)";
    newCircle.v7("") "circmargin(0.05in)";
    v1.c - u1.c = anchor2 - anchor1;
    v2.c - u2.c = anchor2 - anchor1;
    v3.c - u3.c = anchor2 - anchor1;
    v4.c - u4.c = anchor2 - anchor1;
    v5.c - u5.c = anchor2 - anchor1;
    v6.c - u6.c = anchor2 - anchor1;
    v7.c - u7.c = anchor2 - anchor1;
    % ncline(v5)(v6);
    % ncline(v5)(v7);

    numeric r[];
    r1 = xpart(v5.e - v5.w);
    r2 = xpart(ostem.e - ostem.w);
    ostem.c - v5.s = (v5.c - v5.s) * (r2/r1);
    D.c = .5[v6.c,v7.c];
    % D.ne - D.nw = (2unit2, 0);
    % D.ne - D.se = (0, 0.5unit2);
    drawObj(ostem, D);
    for i = 1 upto 7: drawObj(v[i]); endfor
    ncline(v1)(v2);
    ncline(v1)(v3);
    ncline(v2)(v4);
    ncline(v2)(v5);
    ncline(v3)(v4);
    ncline(v4)(v5);
    drawzzarrow(ostem, D);

    label(btex $u$ etex, v5.c shifted (0.25unit2,-0.18unit2));
    label.lft(btex $\mathcal{O}^{stem}$ etex, ostem.w);
    label.lft(btex $\mathcal{D}$ etex, D.w);

    %%%%%%%%%%%
    %% figure 3
    %%%%%%%%%%%
    % a: O^{stem}
    % b: D1
    % c: D2
    % d: D3
    % e: D4
    % g: D5
    newCircle.a(btex $\mathcal{O}^{stem}$ etex)
        "circmargin(0.4in)","framestyle(dashed evenly)";
    newCircle.b(btex $\mathcal{D}_{1}$ etex)
        "circmargin(0.1in)","framestyle(dashed evenly)";
    newCircle.c(btex $\mathcal{D}_{2}$ etex)
        "circmargin(0.2in)","framestyle(dashed evenly)";
    newCircle.d(btex $\mathcal{D}_{3}$ etex)
        "circmargin(0.05in)","framestyle(dashed evenly)";
    newCircle.e(btex $\mathcal{D}_{4}$ etex)
        "circmargin(0.3in)","framestyle(dashed evenly)";
    newCircle.g(btex $\mathcal{D}_{5}$ etex)
        "circmargin(0.25in)","framestyle(dashed evenly)";

    a.c = anchor3 shifted (1.2in, -0.6in);
    b.c - a.c = (-1in,-.8in);
    c.c - a.c = (1.5in, -0.2in);
    d.c - c.c = (-0.7in, -0.5in);
    e.c - c.c = (1.2in, 0.4in);
    g.c - c.c = (1in, -0.6in);

    drawObj(a, b, c, d, e, g);
    drawzzarrow(a,b);
    drawzzarrow(a,c);
    drawzzarrow(c,d);
    drawzzarrow(c,e);
    drawzzarrow(c,g);

    z01 = (a.c--b.c) intersectionpoint BpathCircle(a);
    z02 = (a.c--c.c) intersectionpoint BpathCircle(a);
    z23 = (c.c--d.c) intersectionpoint BpathCircle(c);
    z24 = (c.c--e.c) intersectionpoint BpathCircle(c);
    z25 = (c.c--g.c) intersectionpoint BpathCircle(c);
    label(btex $u_1$ etex, z01-(0,0.2in));
    label(btex $u_2$ etex, z02+(0.12in,0.12in));
    label(btex $u_3$ etex, z23-(0,0.2in));
    label(btex $u_4$ etex, z24+(0,0.2in));
    label(btex $u_5$ etex, z25-(0,0.2in));
endfig;
end;
