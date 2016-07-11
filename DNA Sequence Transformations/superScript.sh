#/bin/bash

#Recompile all files before running timing function
gcc -g jsonTitle.c -o jsonTitle
gcc -g jsonData.c superOptimizer2.c -o superOptimizer2
gcc -g jsonToCSV.c -o jsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/superInputTests/$today

runs=10
maxNumInputs=2
fileSize=10000
#Run superOptimize for 100 times each for 1 through 200 inputs and store in timeStats.txt
#Add new line character inbetween files when appending

for ((i=1; i<(maxNumInputs+1); ++i))
do
	for((inputSet=1; inputSet<(runs+1); ++inputSet))
		do
			echo "input set number $inputSet is being printed"
			./superOptimizer2 $i $fileSize
			if [ $inputSet -eq $runs ]
			then
				./jsonTitle temp1.json "Input Set $inputSet"
			else	
				./jsonTitle temp1.json "Input Set $inputSet" "-c"
				echo "" >> temp1.json
			fi
			
			cat temp1.json >> temp2.json
		done
	if [ $i -eq $maxNumInputs ]
	then
		./jsonTitle temp2.json "$i input(s)"
	else
		./jsonTitle temp2.json "$i input(s)" "-c"
		echo "" >> temp2.json
		rm temp1.json
	fi
	
	cat  temp2.json >> ./tests/superInputTests/$today/superInputTimeStats.json
	rm temp2.json
	
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





