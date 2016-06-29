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
set ylabel "transform time"
plot graph with lines
