#/bin/bash 

#path to today's folder
graph=ARG1

#name of graph output file
graphName=ARG2

#path to the png created
outfile=ARG3

set term png
set output outfile
set title graphName
set xlabel "number of bases"
set ylabel "transform time (usec)"
f(x)=a*x+b
fit f(x) graph u 1:2 via a,b
title_f(a,b)=sprintf('f(x)=%.2fx+%.2f',a,b)
plot graph with linespoints,f(x) t title_f(a,b)
