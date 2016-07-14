#/bin/bash

#Recompile all files before running timing function
gcc -g jsonTitle.c -o jsonTitle
gcc -g jsonData.c superOptimizer2.c -o superOptimizer2
gcc -g superJsonToCSV.c -o superJsonToCSV
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/superTests/$today

folder="./tests/superTests/$today"
runs=100
maxNumInputs=3
fileSize=10000
numInputSets=10
k=3
#Run superOptimize for 100 times each for 1 through 200 inputs and store in timeStats.txt
#Add new line character inbetween files when appending

./jsonSystemStats $folder/superInputTimeStats.json

for ((i=1; i<(maxNumInputs+1); ++i))
do
	for((inputSet=1; inputSet< (numInputSets+1); ++inputSet))
		do
			echo "input set number $inputSet is being printed"
			./superOptimizer2 $i $fileSize $(( RANDOM )) $runs
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
	
	cat  temp2.json >> $folder/superInputTimeStats.json
	rm temp2.json
	
done

#Add Title to superInputTimeStats.txt file
./jsonTitle $folder/superInputTimeStats.json "superOptimizer" "-f"

./superJsonToCSV $folder/superInputTimeStats.json $folder/superInputTimeStats.csv $folder/superNumOpTimeStats.csv $maxNumInputs $numInputSets $runs $k
inputPlotFile="superInputTimeStats.csv"
numOpPlotFile="superNumOpTimeStats.csv"

output1="superInRun.png"
output2="superInEval.png"
output3="superNumOpRun.png"
output4="superNumOpEval.png"

inputDataFile="$folder/$inputPlotFile"
numOpDataFile="$folder/$numOpPlotFile"

touch $folder/$output1
touch $folder/$output2
touch $folder/$output3
touch $folder/$output4


outpath1="$folder/$output1"
outpath2="$folder/$output2"
outpath3="$folder/$output3"
outpath4="$folder/$output4"

inputXlabel="Number of inputs"
numOpXlabel="Number of operations"

graphTitle1="Input Run Time"
graphTitle2="Input Evaluation Time"
graphTitle3="Number of Operations Run Time"
graphTitle4="Number of Operations Evaluation Time"


gnuplot -c plotExpScript.sh $inputDataFile "$graphTitle1" $outpath1 "$inputXlabel" "1" "2"
gnuplot -c plotLineScript.sh $inputDataFile "$graphTitle2" $outpath2 "$inputXlabel" "1" "3"

gnuplot -c plotExpScript.sh $numOpDataFile "$graphTitle3" $outpath3 "$numOpXlabel" "1" "2"
gnuplot -c plotLineScript.sh $numOpDataFile "$graphTitle4" $outpath4 "$numOpXlabel" "1" "3"





