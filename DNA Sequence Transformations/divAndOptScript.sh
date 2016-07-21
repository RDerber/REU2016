#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc -g jsonData.c divideAndOptimize.c -o divideAndOptimize
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g divAndOptJsonToCSV.c -o divAndOptJsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/divAndOptTests/$today

numInputFiles=0
runs=100
evals=100
k=3
folder="./tests/divAndOptTests/$today"

./jsonSystemStats $folder/divAndOptTimeStats.json

#Run divideAndOptimize divAndOpt Tests and Store in timeStats.txt
./divideAndOptimize tests/divAndOptTests/key1.txt tests/divAndOptTests/key1In.txt $folder/key1Out.txt $runs
./jsonTitle timing.json "key 1" "-c"
# Add new line character inbetween files when appending
echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

./divideAndOptimize tests/divAndOptTests/key2.txt tests/divAndOptTests/key2In.txt $folder/key2Out.txt $runs
./jsonTitle timing.json "key 2" "-c"
echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

./divideAndOptimize tests/divAndOptTests/key3.txt tests/divAndOptTests/key3In.txt $folder/key3Out.txt $runs
./jsonTitle timing.json "key 3" "-c"
echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

./divideAndOptimize tests/divAndOptTests/key4.txt tests/divAndOptTests/key4In.txt $folder/key4Out.txt $runs
./jsonTitle timing.json "key 4" "-c"
echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

./divideAndOptimize tests/divAndOptTests/key5.txt tests/divAndOptTests/key5In.txt $folder/key5Out.txt $runs
./jsonTitle timing.json "key 5" "-c"
echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

./divideAndOptimize tests/divAndOptTests/key6.txt tests/divAndOptTests/key6In.txt $folder/key6Out.txt $runs
./jsonTitle timing.json "key 6" "-c"
#echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

./divideAndOptimize tests/divAndOptTests/key7.txt tests/divAndOptTests/key7In.txt $folder/key7Out.txt $runs
./jsonTitle timing.json "key 7" 
#echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

#Add Title to divAndOptTimeStats.txt file
./jsonTitle $folder/divAndOptTimeStats.json "divideAndOptimize" "-f"

./divAndOptJsonToCSV $folder/divAndOptTimeStats.json $folder/divAndOptTimeStats.csv $numInputFiles $runs $evals $k

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




