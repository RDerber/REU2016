#/bin/bash 

#path to CSV FILE
graph1=ARG1

#graph title
graphTitle=ARG2

#path to the png created
outputPath=ARG3

#what to set xlabel as
myXLabel=ARG4

#column of x values
xvals1=ARG5 + 0

#column of y values
yvals1=ARG6 + 0

xvals2=ARG7 + 0

yvals2=ATG8 + 0

#path to second csv file
graph2=ARG9

set term png
set datafile separator ","
set output outputPath
set title graphTitle


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
