#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc jsonData.c divideAndOptimize.c -o divideAndOptimize
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
k=3
folder="./tests/divAndOptTests/$today"

./jsonSystemStats $folder/divAndOptTimeStats.json

#Run divideAndOptimize divAndOpt Tests and Store in timeStats.txt
./divideAndOptimize tests/divAndOptTests/key1.txt tests/divAndOptTests/key1In.txt $folder/key1Out.txt $runs
./jsonTitle timing.json "key 1" "-c"
echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))
 # Add new line character inbetween files when appending

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
./jsonTitle timing.json "key 6" 
#echo "" >> timing.json
cat timing.json >> $folder/divAndOptTimeStats.json
((numInputFiles++))

#./divideAndOptimize divAndOptAminoAcidKey.txt tests/divAndOptTests/fasta100k.txt $folder/100kOut.txt $runs
#./jsonTitle timing.json "100k bases" "-c"
#echo "" >> timing.json
#cat timing.json >> ./$folder/divAndOptTimeStats.json

#./divideAndOptimize divAndOptAminoAcidKey.txt tests/divAndOptTests/fasta500k.txt $folder/500kOut.txt $runs
#./jsonTitle timing.json "500k bases" "-c"
#echo "" >> timing.json
#cat timing.json >> ./$folder/divAndOptTimeStats.json

#./divideAndOptimize divAndOptAminoAcidKey.txt tests/divAndOptTests/fasta1mil.txt $folder/1milOut.txt $runs
#./jsonTitle timing.json "1mil bases"
#echo "" >> timing.json
#cat timing.json >> ./$folder/divAndOptTimeStats.json

#Add Title to divAndOptTimeStats.txt file
./jsonTitle $folder/divAndOptTimeStats.json "divideAndOptimize" "-f"

./divAndOptJsonToCSV $folder/divAndOptTimeStats.json $folder/divAndOptTimeStats.csv $numInputFiles $runs $k

plotfile="divAndOptTimeStats.csv"
output="divAndOpt.png"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of Key Inputs"
graphTitle="Divide And Optimize Key Mapping"
xvals=1
yvals=2

gnuplot -c plotLogScript.sh $graph "$graphTitle" $outpath "$xlabel" $xvals $yvals




