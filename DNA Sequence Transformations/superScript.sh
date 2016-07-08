#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c superOptimizer.c -o superOptimizer

if [ $1 != NULL ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/superInputTests/$today

#Run superOptimize for 100 times each for 1 through 200 inputs and store in timeStats.txt
#Add new line character inbetween files when appending
for i in {1..2}
do
		./superOptimizer $i
		./jsonTitle timing.json "$i input(s)" "-c"
		echo "" >> timing.json
		cat timing.json >> ./tests/superInputTests/$today/superInputTimeStats.json

done

#Add Title to superInputTimeStats.txt file
./jsonTitle ./tests/superInputTests/$today/superInputTimeStats.json "superOptimizer"  

./jsonToCSV ./tests/superInputTests/$today/superInputTimeStats.json ./tests/superInputTests/$today/superInputTimeStats.csv
plotfile="superInputTimeStats.csv"
output="super.png"
folder="./tests/superInputTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of inputs"
graphTitle="SuperOptimizer Performance"

gnuplot -c plotExpScript.sh $graph "$graphTitle" $outpath "$xlabel"





