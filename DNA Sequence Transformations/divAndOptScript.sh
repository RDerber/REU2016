#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc -g keyGen.c -o keyGen
gcc -g randFromKey.c -o randFromKey
gcc -g jsonData.c divideAndOptimize.c -o divideAndOptimize
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g divAndOptJsonToCSV.c -o divAndOptJsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

#Make results directory and remove old files
mkdir ./tests/divAndOptTests/$today
rm temp1.json
rm temp2.json

#Adjustable timing parameters 

numKeys=20	#This is the numer of different sized keys tested (each with it's own corresponding input file)
		#cannot be much higher than 50 for good run time. Will break if higher than 63 (63*2 = 126. 127 is max acceptible 
		#random ascii value
		
numInputSets=1 #This is the number of files tested for each key size 

runs=20 #The number of times the divAndOp Sequence finder is run
evals=50 #The number of times the found sequence is used to translate an input file

k=3 #The lowest K timing values to be averaged in the timing report to make the csv file

folder="./tests/divAndOptTests/$today"

#Test divAndOpt for accuracy
./divideAndOptimize tests/divAndOptTests/testKey.txt tests/divAndOptTests/testKeyIn.txt $folder/testOut.txt 1 1

DIFF=$(diff -a tests/divAndOptTests/testCheck.txt $folder/testOut.txt)
if [ "$DIFF" != "" ] 
then
	echo "DivideAndOptimize is no longer correct. Terminating timing script."
	exit
fi

./jsonSystemStats $folder/divAndOptTimeStats.json
for ((i=1; i<(numKeys+1); ++i))
do
	for((inputSet=1; inputSet<(numInputSets+1); ++inputSet))
		do
			#Create random key
			./keyGen $(( 2 * $i )) tests/divAndOptTests/ $(( RANDOM ))
			
			#Create random key input file
			./randFromKey tests/divAndOptTests/key"$i".txt 10000 tests/divAndOptTests/key"$i"In.txt
			
			#status
			echo "input set number $inputSet is being evaluated"
			echo $(date +%Y-%m-%d-%T)
			echo "Current Key #: $i"
			echo ""
			#Run divideAndOptimize divAndOpt Tests and Store in temp1.json
			./divideAndOptimize tests/divAndOptTests/key"$i".txt tests/divAndOptTests/key"$i"In.txt $folder/key"$i"Out.txt $runs $evals
		
			if [ "$inputSet" = "$numInputSets" ]
			then
					./jsonTitle temp1.json "Input Set $inputSet"
			else	
					./jsonTitle temp1.json "Input Set $inputSet" "-c"
					echo "" >> temp1.json
			fi
				
			cat temp1.json >> temp2.json
		done
		
		
	if [ "$i" = "$numKeys" ]
	then
		./jsonTitle temp2.json "key $i" 
		echo "then!"
	else
		./jsonTitle temp2.json "key $i" "-c"
		# Add new line character inbetween files when appending
		echo "" >> temp2.json
		rm temp1.json
	fi
	
	cat temp2.json >> $folder/divAndOptTimeStats.json
	rm temp2.json
done
	
	

#Add Title to divAndOptTimeStats.txt file
./jsonTitle $folder/divAndOptTimeStats.json "divideAndOptimize" "-f"

./divAndOptJsonToCSV $folder/divAndOptTimeStats.json $folder/divAndOptTimeStats.csv $numKeys $numInputSets $runs $evals $k

inputPlotFile="divAndOptTimeStats.csv"

output1="divAndOptRun.png"
output2="divAndOptEval.png"


inputDataFile="$folder/$inputPlotFile"

touch $folder/$output1
touch $folder/$output2

outpath1="$folder/$output1"
outpath2="$folder/$output2"

inputXlabel="Size of Key"

graphTitle1="Divide And Optimize Key Mapping"
graphTitle2="Divide And Optimize Evaluation Time"


gnuplot -c plotPolyScript.sh $inputDataFile "$graphTitle1" $outpath1 "$inputXlabel" "1" "2"
gnuplot -c plotLogScript.sh $inputDataFile "$graphTitle2" $outpath2 "$inputXlabel" "1" "3"




