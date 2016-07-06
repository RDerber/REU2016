#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c divideAndOptimize.c -o divideAndOptimize

if [ ! [ $1 = NULL ] ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/divAndOptTests/$today

#Run divideAndOptimize divAndOpt Tests and Store in timeStats.txt
./divideAndOptimize tests/divAndOptTests/key1.txt tests/divAndOptTests/key1In.txt tests/divAndOptTests/$today/key1Out.txt 100 3
./jsonTitle timing.json "key1" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json
 # Add new line character inbetween files when appending

./divideAndOptimize tests/divAndOptTests/key2.txt tests/divAndOptTests/key2In.txt tests/divAndOptTests/$today/key2Out.txt 100 3
./jsonTitle timing.json "key2" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

./divideAndOptimize tests/divAndOptTests/key3.txt tests/divAndOptTests/key3In.txt tests/divAndOptTests/$today/key3Out.txt 100 3
./jsonTitle timing.json "key3" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

./divideAndOptimize tests/divAndOptTests/key4.txt tests/divAndOptTests/key4In.txt tests/divAndOptTests/$today/key4Out.txt 100 3
./jsonTitle timing.json "key4" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

./divideAndOptimize tests/divAndOptTests/key5.txt tests/divAndOptTests/key5In.txt tests/divAndOptTests/$today/key5Out.txt 100 3
./jsonTitle timing.json "key5" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

./divideAndOptimize tests/divAndOptTests/key6.txt tests/divAndOptTests/key6In.txt tests/divAndOptTests/$today/key6Out.txt 100 3
./jsonTitle timing.json "key6" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

#./divideAndOptimize divAndOptAminoAcidKey.txt tests/divAndOptTests/fasta100k.txt tests/divAndOptTests/$today/100kOut.txt 100 3
#./jsonTitle timing.json "100k bases" "-c"
#echo "" >> timing.json
#cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

#./divideAndOptimize divAndOptAminoAcidKey.txt tests/divAndOptTests/fasta500k.txt tests/divAndOptTests/$today/500kOut.txt 100 3
#./jsonTitle timing.json "500k bases" "-c"
#echo "" >> timing.json
#cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

#./divideAndOptimize divAndOptAminoAcidKey.txt tests/divAndOptTests/fasta1mil.txt tests/divAndOptTests/$today/1milOut.txt 100 3
#./jsonTitle timing.json "1mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/divAndOptTests/$today/divAndOptTimeStats.json

#Add Title to divAndOptTimeStats.txt file
./jsonTitle ./tests/divAndOptTests/$today/divAndOptTimeStats.json "divideAndOptimize divAndOptAminoAcidKey.txt"  

./jsonToCSV ./tests/divAndOptTests/$today/divAndOptTimeStats.json ./tests/divAndOptTests/$today/divAndOptTimeStats.csv
plotfile="divAndOptTimeStats.csv"
output="divAndOpt.png"
folder="./tests/divAndOptTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of Key Inputs"
graphTitle="Divide And Optimize Key Mapping"

gnuplot -c plotLogScript.sh $graph "$graphTitle" $outpath "$xlabel"




