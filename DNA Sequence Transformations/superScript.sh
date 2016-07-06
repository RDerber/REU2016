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

mkdir ./tests/superTests/$today

#Run superOptimize for 100 times each for 1 through 200 inputs and store in timeStats.txt
#Add new line character inbetween files when appending
for i in {1..4}
do
		./superOptimizer $i
		./jsonTitle timing.json "$i input(s)" "-c"
		echo "" >> timing.json
		cat timing.json >> ./tests/superTests/$today/superTimeStats.json

done

#Add Title to superTimeStats.txt file
./jsonTitle ./tests/superTests/$today/superTimeStats.json "superOptimizer"  

./jsonToCSV ./tests/superTests/$today/superTimeStats.json ./tests/superTests/$today/superTimeStats.csv
plotfile="superTimeStats.csv"
output="super.png"
folder="./tests/superTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of inputs"

gnuplot -c plotLineScript.sh $graph $output $outpath "$xlabel"





