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
    unit := 0.4in;

    anchor01 = origin;
    anchor02 - anchor01 = (1.7unit, 0);
    anchor03 - anchor02 = (1.7unit, 0);
    anchor04 - anchor03 = (1.7unit, 0);
    anchor05 - anchor04 = (1.7unit, 0);
    anchor06 - anchor05 = (1.7unit, 0);
    anchor07 - anchor06 = (1.7unit, 0);
    anchor08 - anchor07 = (1.7unit, 0);
    anchor09 - anchor08 = (1.7unit, 0);
    anchor10 - anchor09 = (1.7unit, 0);
    anchor11 - anchor10 = (1.7unit, 0);
    anchor12 - anchor11 = (1.7unit, 0);
    anchor13 - anchor12 = (1.7unit, 0);
    anchor14 - anchor13 = (1.7unit, 0);
    anchor15 - anchor14 = (1.7unit, 0);

    %%%%%%%% 01
    newCircle.u011("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u012("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u013("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u014("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u015("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u016("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u017("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u011.c = anchor01;
    u012.c - u011.c = u014.c - u012.c = u015.c - u013.c = u016.c - u15.c = (-0.4unit, -0.6unit);
    u013.c - u011.c = u017.c - u015.c = (0.4unit, -0.6unit);

    for i = 011 upto 017: drawObj(u[i]); endfor
    ncline(u011)(u012);
    ncline(u011)(u013);
    ncline(u012)(u014);
    ncline(u012)(u015);
    ncline(u013)(u015);
    ncline(u015)(u016);
    ncline(u015)(u017);

    %%%%%%%% 02
    newCircle.u021("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u022("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u023("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u024("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u025("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u026("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u027("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u021.c - u011.c = anchor02 - anchor01;
    u022.c - u012.c = anchor02 - anchor01;
    u023.c - u013.c = anchor02 - anchor01;
    u024.c - u014.c = anchor02 - anchor01;
    u025.c - u015.c = anchor02 - anchor01;
    u026.c - u016.c = anchor02 - anchor01;
    u027.c - u017.c = anchor02 - anchor01;

    for i = 021 upto 027: drawObj(u[i]); endfor
    ncline(u021)(u022);
    ncline(u021)(u023);
    ncline(u022)(u024);
    ncline(u022)(u025);
    ncline(u023)(u025);
    ncline(u025)(u026);
    ncline(u025)(u027);

    %%%%%%%% 03
    newCircle.u031("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u032("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u033("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u034("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u035("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u036("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u037("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u031.c - u011.c = anchor03 - anchor01;
    u032.c - u012.c = anchor03 - anchor01;
    u033.c - u013.c = anchor03 - anchor01;
    u034.c - u014.c = anchor03 - anchor01;
    u035.c - u015.c = anchor03 - anchor01;
    u036.c - u016.c = anchor03 - anchor01;
    u037.c - u017.c = anchor03 - anchor01;

    for i = 031 upto 037: drawObj(u[i]); endfor
    ncline(u031)(u032);
    ncline(u031)(u033);
    ncline(u032)(u034);
    ncline(u032)(u035);
    ncline(u033)(u035);
    ncline(u035)(u036);
    ncline(u035)(u037);

    %%%%%%%% 04
    newCircle.u041("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u042("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u043("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u044("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u045("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u046("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u047("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u041.c - u011.c = anchor04 - anchor01;
    u042.c - u012.c = anchor04 - anchor01;
    u043.c - u013.c = anchor04 - anchor01;
    u044.c - u014.c = anchor04 - anchor01;
    u045.c - u015.c = anchor04 - anchor01;
    u046.c - u016.c = anchor04 - anchor01;
    u047.c - u017.c = anchor04 - anchor01;

    for i = 041 upto 047: drawObj(u[i]); endfor
    ncline(u041)(u042);
    ncline(u041)(u043);
    ncline(u042)(u044);
    ncline(u042)(u045);
    ncline(u043)(u045);
    ncline(u045)(u046);
    ncline(u045)(u047);

    %%%%%%%% 05
    newCircle.u051("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u052("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u053("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u054("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u055("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u056("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u057("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u051.c - u011.c = anchor05 - anchor01;
    u052.c - u012.c = anchor05 - anchor01;
    u053.c - u013.c = anchor05 - anchor01;
    u054.c - u014.c = anchor05 - anchor01;
    u055.c - u015.c = anchor05 - anchor01;
    u056.c - u016.c = anchor05 - anchor01;
    u057.c - u017.c = anchor05 - anchor01;

    for i = 051 upto 057: drawObj(u[i]); endfor
    ncline(u051)(u052);
    ncline(u051)(u053);
    ncline(u052)(u054);
    ncline(u052)(u055);
    ncline(u053)(u055);
    ncline(u055)(u056);
    ncline(u055)(u057);

    %%%%%%%% 06
    newCircle.u061("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u062("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u063("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u064("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u065("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u066("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u067("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u061.c - u011.c = anchor06 - anchor01;
    u062.c - u012.c = anchor06 - anchor01;
    u063.c - u013.c = anchor06 - anchor01;
    u064.c - u014.c = anchor06 - anchor01;
    u065.c - u015.c = anchor06 - anchor01;
    u066.c - u016.c = anchor06 - anchor01;
    u067.c - u017.c = anchor06 - anchor01;

    for i = 061 upto 067: drawObj(u[i]); endfor
    ncline(u061)(u062);
    ncline(u061)(u063);
    ncline(u062)(u064);
    ncline(u062)(u065);
    ncline(u063)(u065);
    ncline(u065)(u066);
    ncline(u065)(u067);

    %%%%%%%% 07
    newCircle.u071("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u072("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u073("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u074("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u075("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u076("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u077("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u071.c - u011.c = anchor07 - anchor01;
    u072.c - u012.c = anchor07 - anchor01;
    u073.c - u013.c = anchor07 - anchor01;
    u074.c - u014.c = anchor07 - anchor01;
    u075.c - u015.c = anchor07 - anchor01;
    u076.c - u016.c = anchor07 - anchor01;
    u077.c - u017.c = anchor07 - anchor01;

    for i = 071 upto 077: drawObj(u[i]); endfor
    ncline(u071)(u072);
    ncline(u071)(u073);
    ncline(u072)(u074);
    ncline(u072)(u075);
    ncline(u073)(u075);
    ncline(u075)(u076);
    ncline(u075)(u077);

    %%%%%%%% 08
    newCircle.u081("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u082("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u083("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u084("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u085("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u086("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u087("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u081.c - u011.c = anchor08 - anchor01;
    u082.c - u012.c = anchor08 - anchor01;
    u083.c - u013.c = anchor08 - anchor01;
    u084.c - u014.c = anchor08 - anchor01;
    u085.c - u015.c = anchor08 - anchor01;
    u086.c - u016.c = anchor08 - anchor01;
    u087.c - u017.c = anchor08 - anchor01;

    for i = 081 upto 087: drawObj(u[i]); endfor
    ncline(u081)(u082);
    ncline(u081)(u083);
    ncline(u082)(u084);
    ncline(u082)(u085);
    ncline(u083)(u085);
    ncline(u085)(u086);
    ncline(u085)(u087);

    %%%%%%%% 09
    newCircle.u091("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u092("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u093("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u094("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u095("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u096("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u097("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u091.c - u011.c = anchor09 - anchor01;
    u092.c - u012.c = anchor09 - anchor01;
    u093.c - u013.c = anchor09 - anchor01;
    u094.c - u014.c = anchor09 - anchor01;
    u095.c - u015.c = anchor09 - anchor01;
    u096.c - u016.c = anchor09 - anchor01;
    u097.c - u017.c = anchor09 - anchor01;

    for i = 091 upto 097: drawObj(u[i]); endfor
    ncline(u091)(u092);
    ncline(u091)(u093);
    ncline(u092)(u094);
    ncline(u092)(u095);
    ncline(u093)(u095);
    ncline(u095)(u096);
    ncline(u095)(u097);

    %%%%%%%% 10
    newCircle.u101("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u102("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u103("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u104("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u105("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u106("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u107("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u101.c - u011.c = anchor10 - anchor01;
    u102.c - u012.c = anchor10 - anchor01;
    u103.c - u013.c = anchor10 - anchor01;
    u104.c - u014.c = anchor10 - anchor01;
    u105.c - u015.c = anchor10 - anchor01;
    u106.c - u016.c = anchor10 - anchor01;
    u107.c - u017.c = anchor10 - anchor01;

    for i = 101 upto 107: drawObj(u[i]); endfor
    ncline(u101)(u102);
    ncline(u101)(u103);
    ncline(u102)(u104);
    ncline(u102)(u105);
    ncline(u103)(u105);
    ncline(u105)(u106);
    ncline(u105)(u107);

    %%%%%%%% 11
    newCircle.u111("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u112("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u113("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u114("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u115("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u116("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u117("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";

    u111.c - u011.c = anchor11 - anchor01;
    u112.c - u012.c = anchor11 - anchor01;
    u113.c - u013.c = anchor11 - anchor01;
    u114.c - u014.c = anchor11 - anchor01;
    u115.c - u015.c = anchor11 - anchor01;
    u116.c - u016.c = anchor11 - anchor01;
    u117.c - u017.c = anchor11 - anchor01;

    for i = 111 upto 117: drawObj(u[i]); endfor
    ncline(u111)(u112);
    ncline(u111)(u113);
    ncline(u112)(u114);
    ncline(u112)(u115);
    ncline(u113)(u115);
    ncline(u115)(u116);
    ncline(u115)(u117);

    %%%%%%%% 12
    newCircle.u121("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u122("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u123("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u124("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u125("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u126("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u127("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";

    u121.c - u011.c = anchor12 - anchor01;
    u122.c - u012.c = anchor12 - anchor01;
    u123.c - u013.c = anchor12 - anchor01;
    u124.c - u014.c = anchor12 - anchor01;
    u125.c - u015.c = anchor12 - anchor01;
    u126.c - u016.c = anchor12 - anchor01;
    u127.c - u017.c = anchor12 - anchor01;

    for i = 121 upto 127: drawObj(u[i]); endfor
    ncline(u121)(u122);
    ncline(u121)(u123);
    ncline(u122)(u124);
    ncline(u122)(u125);
    ncline(u123)(u125);
    ncline(u125)(u126);
    ncline(u125)(u127);

    %%%%%%%% 13
    newCircle.u131("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u132("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u133("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u134("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u135("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u136("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u137("") "circmargin(0.05in)", "framewidth(1.5bp)";

    u131.c - u011.c = anchor13 - anchor01;
    u132.c - u012.c = anchor13 - anchor01;
    u133.c - u013.c = anchor13 - anchor01;
    u134.c - u014.c = anchor13 - anchor01;
    u135.c - u015.c = anchor13 - anchor01;
    u136.c - u016.c = anchor13 - anchor01;
    u137.c - u017.c = anchor13 - anchor01;

    for i = 131 upto 137: drawObj(u[i]); endfor
    ncline(u131)(u132);
    ncline(u131)(u133);
    ncline(u132)(u134);
    ncline(u132)(u135);
    ncline(u133)(u135);
    ncline(u135)(u136);
    ncline(u135)(u137);

    %%%%%%%% 14
    newCircle.u141("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u142("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u143("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u144("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u145("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u146("") "circmargin(0.05in)", "framewidth(1.5bp)";
    newCircle.u147("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";

    u141.c - u011.c = anchor14 - anchor01;
    u142.c - u012.c = anchor14 - anchor01;
    u143.c - u013.c = anchor14 - anchor01;
    u144.c - u014.c = anchor14 - anchor01;
    u145.c - u015.c = anchor14 - anchor01;
    u146.c - u016.c = anchor14 - anchor01;
    u147.c - u017.c = anchor14 - anchor01;

    for i = 141 upto 147: drawObj(u[i]); endfor
    ncline(u141)(u142);
    ncline(u141)(u143);
    ncline(u142)(u144);
    ncline(u142)(u145);
    ncline(u143)(u145);
    ncline(u145)(u146);
    ncline(u145)(u147);

    %%%%%%%% 15
    newCircle.u151("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u152("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u153("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u154("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u155("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u156("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";
    newCircle.u157("") "circmargin(0.05in)", "framewidth(1.5bp)", "filled(true)", "fillcolor((0.000,0.584,0.867))";

    u151.c - u011.c = anchor15 - anchor01;
    u152.c - u012.c = anchor15 - anchor01;
    u153.c - u013.c = anchor15 - anchor01;
    u154.c - u014.c = anchor15 - anchor01;
    u155.c - u015.c = anchor15 - anchor01;
    u156.c - u016.c = anchor15 - anchor01;
    u157.c - u017.c = anchor15 - anchor01;

    for i = 151 upto 157: drawObj(u[i]); endfor
    ncline(u151)(u152);
    ncline(u151)(u153);
    ncline(u152)(u154);
    ncline(u152)(u155);
    ncline(u153)(u155);
    ncline(u155)(u156);
    ncline(u155)(u157);

endfig;
end;
