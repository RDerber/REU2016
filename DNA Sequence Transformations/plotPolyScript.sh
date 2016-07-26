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
xvals=ARG5 + 0

#column of y values
yvals=ARG6 + 0

set term png
set datafile separator ","
set output outputPath
set title graphTitle

print graph
print xvals
print yvals

set xlabel myXLabel
set ylabel "Transform Time (µsec)"
set key center bottom right title box 3
a=.01
b=.1
c=.01
d=.01
e=1
f(x)=c*x**3+e
fit f(x) graph using xvals:yvals via c,e
title_f(c,e)=sprintf('f(x)=%.2fx^3+%.2f',c,e)
plot graph u xvals:yvals notitle with linespoints,f(x) t title_f(c,e)
