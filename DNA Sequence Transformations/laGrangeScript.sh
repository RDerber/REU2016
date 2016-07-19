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

numInputFiles=0
runs=100
k=3
folder="./tests/laGrangeTests/$today"

./jsonSystemStats $folder/laGrangeTimeStats.json

#Run LaGrangeGen laGrange Tests and Store in timeStats.txt
./LaGrangeGen tests/laGrangeTests/key1.txt tests/laGrangeTests/key1In.txt $folder/key1Out.txt $runs
./jsonTitle temp1.json "key 1" "-c"
# Add new line character inbetween files when appending
echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

./LaGrangeGen tests/laGrangeTests/key2.txt tests/laGrangeTests/key2In.txt $folder/key2Out.txt $runs
./jsonTitle temp1.json "key 2" "-c"
echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

./LaGrangeGen tests/laGrangeTests/key3.txt tests/laGrangeTests/key3In.txt $folder/key3Out.txt $runs
./jsonTitle temp1.json "key 3" "-c"
echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

./LaGrangeGen tests/laGrangeTests/key4.txt tests/laGrangeTests/key4In.txt $folder/key4Out.txt $runs
./jsonTitle temp1.json "key 4" "-c"
echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

./LaGrangeGen tests/laGrangeTests/key5.txt tests/laGrangeTests/key5In.txt $folder/key5Out.txt $runs
./jsonTitle temp1.json "key 5" "-c"
echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

./LaGrangeGen tests/laGrangeTests/key6.txt tests/laGrangeTests/key6In.txt $folder/key6Out.txt $runs
./jsonTitle temp1.json "key 6" 
#echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

./LaGrangeGen tests/laGrangeTests/key7.txt tests/laGrangeTests/key7In.txt $folder/key7Out.txt $runs
./jsonTitle temp1.json "key 7" 
#echo "" >> temp1.json
cat temp1.json >> $folder/laGrangeTimeStats.json
((numInputFiles++))

#Add Title to laGrangeTimeStats.txt file
./jsonTitle $folder/laGrangeTimeStats.json "LaGrangeGen" "-f"

./laGrangeJsonToCSV $folder/laGrangeTimeStats.json $folder/laGrangeTimeStats.csv $numInputFiles $runs $k

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



