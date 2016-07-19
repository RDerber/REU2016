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
set xlabel myXLabel
set ylabel "Transform Time (µsec)"
set key center bottom right title box 3
f(x)=a*x+b*x*log(x)/log(2)+c
fit f(x) graph u xvals:yvals via a,b,c
title_f(a,b,c)=sprintf('f(x)=%.2fx+%.2fxlog(x)/log(2)+%.2f',a,b,c)
plot graph u xvals:yvals notitle with linespoints,f(x) t title_f(a,b,c)
