#/bin/bash 

#path to today's folder
graph=ARG1

#graph title
graphTitle=ARG2

#path to the png created
outputPath=ARG3

set term png
set output outputPath
set title graphTitle

if (ARG4 eq "" ) set xlabel "Number of Bases" ; else set xlabel ARG4
set ylabel "Transform Time (µsec)"
set key center top left title box 3
set xtics rotate by -90
f(x)=a*x+b
fit f(x) graph u 1:2 via a,b
title_f(a,b)=sprintf('f(x)=%.2fx+%.2f',a,b)
plot graph with linespoints,f(x) t title_f(a,b)
