#/bin/bash

#Recompile all files before running temp1 function
gcc -g jsonTitle.c -o jsonTitle
gcc -g jsonData.c LaGrangeGen.c -o LaGrangeGen
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g laGrangeJsonToCSV.c -o laGrangeJsonToCSV 

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/laGrangeTests/$today

numKeys=40
runs=10
k=3
folder="./tests/laGrangeTests/$today"

./jsonSystemStats $folder/laGrangeTimeStats.json

#Test LaGrange for accuracy
./LaGrangeGen tests/laGrangeTests/testKey.txt tests/laGrangeTests/testKeyIn.txt $folder/testOut.txt 1

DIFF=$(diff -a tests/laGrangeTests/testCheck.txt $folder/testOut.txt)
if [ "$DIFF" != "" ] 
then
	echo "LaGrangeGen is no longer correct. Terminating timing script."
	exit
fi


rm temp1.json
for((i=1; i<(numKeys+1); ++i))
do
	#Create random key
	./keyGen $(( 2 * $i )) tests/laGrangeTests/ $(( RANDOM ))
	
	#Create random key input file
	./randFromKey tests/laGrangeTests/key"$i".txt 10000 tests/laGrangeTests/key"$i"In.txt
	
	#status
	echo $(date +%Y-%m-%d-%T)
	echo "Current Key #: $i"
	echo ""
	#Run divideAndOptimize laGrange Tests and Store in temp1.json
	./LaGrangeGen tests/laGrangeTests/key"$i".txt tests/laGrangeTests/key"$i"In.txt $folder/key"$i"Out.txt $runs

	
	if [ "$i" = "$numKeys" ]
	then
		./jsonTitle temp1.json "key $i"
	else
		./jsonTitle temp1.json "key $i" "-c"
		# Add new line character inbetween files when appending
		echo "" >> temp1.json
	fi
	
	cat temp1.json >> $folder/laGrangeTimeStats.json
	rm temp1.json
done

#Add Title to laGrangeTimeStats.txt file
./jsonTitle $folder/laGrangeTimeStats.json "LaGrangeGen" "-f"

./laGrangeJsonToCSV $folder/laGrangeTimeStats.json $folder/laGrangeTimeStats.csv $numKeys $runs $k

inputPlotFile="laGrangeTimeStats.csv"

output1="laGrangePolyGen.png"
output2="laGrangeEval.png"


inputDataFile="$folder/$inputPlotFile"

touch $folder/$output1
touch $folder/$output2

outpath1="$folder/$output1"
outpath2="$folder/$output2"

inputXlabel="Size of Key"

graphTitle1="Polynomial Generation Time"
graphTitle2="Evaluation Time"


gnuplot -c plotPolyScript.sh $inputDataFile "$graphTitle1" $outpath1 "$inputXlabel" "1" "2"
gnuplot -c laGrangeEvalPlotScript.sh $inputDataFile "$graphTitle2" $outpath2 "$inputXlabel" "1" "3"



