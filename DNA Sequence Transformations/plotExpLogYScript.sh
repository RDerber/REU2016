#/bin/bash 

# plots an exponential graph of the csv file, with the yaxis on a logarithmic scale

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
set key center top left title box 3
a=100
b=50
n=2
f(x)=a*x+b
fit f(x) graph using xvals:(log($2)) via a,b
title_f(a,b)=sprintf('f(x)=%.2f*x+%.2f',a,b)
plot graph u xvals:(log($2)) notitle with linespoints,f(x) t title_f(a,b)
