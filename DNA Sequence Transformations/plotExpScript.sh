#/bin/bash 

#path to today's folder
graph=ARG1

#graph title
graphTitle=ARG2

#path to the png created
outputPath=ARG3

#what to set xlabel as
myXLabel=ARG4

#column of x values
xvals=ARG5

#column of y values
yvals=ARG6

set term png
set output outputPath
set title graphTitle

set xlabel myXLabel
set ylabel "Transform Time (µsec)"
set key center bottom right title box 3
f(x)=a**x+b
fit f(x) graph u xvals:yvals via a,b
title_f(a,b)=sprintf('f(x)=%.2f^{x}+%.2f',a,b)
plot graph with linespoints,f(x) t title_f(a,b)
