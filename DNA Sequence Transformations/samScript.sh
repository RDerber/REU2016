#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc jsonData.c samTo2Bit.c -o samTo2Bit
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g timingJsonToCSV.c -o timingJsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

#Make results diretory 
mkdir ./tests/samTests/$today

#Adjustable timing parameters 
numInputFiles=0
runs=100
k=3
folder="./tests/samTests/$today"

#Test Fasta for accuracy
./samTo2Bit tests/samTests/samTest.SAM $folder/testOut.txt  $folder/testHeaders.txt $folder/testPositions.txt 1

DIFF=$(diff -a tests/samTests/testCheck.txt $folder/testOut.txt)
if [ "$DIFF" != "" ] 
then
	echo "The SAM to 2Bit conversion is no longer correct. Terminating timing script."
	exit
fi

#Insert System Stats into output file
./jsonSystemStats $folder/sam2BTimeStats.json



#Run SAM To 2 Bit Tests and Store in timeStats.txt
./samTo2Bit tests/samTests/sam100.SAM $folder/100Out.txt $folder/100headers.txt $folder/100positions.txt $runs
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))
 # Add new line character inbetween files when appending

./samTo2Bit tests/samTests/sam500.SAM $folder/500Out.txt $folder/500headers.txt $folder/500positions.txt $runs
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam1k.SAM $folder/1kOut.txt $folder/1kheaders.txt $folder/1kpositions.txt $runs
./jsonTitle timing.json "1000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam5k.SAM $folder/5kOut.txt $folder/5kheaders.txt $folder/5kpositions.txt $runs
./jsonTitle timing.json "5000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam10k.SAM $folder/10kOut.txt $folder/10kheaders.txt $folder/10kpositions.txt $runs
./jsonTitle timing.json "10000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam50k.SAM $folder/50kOut.txt $folder/50kheaders.txt $folder/50kpositions.txt $runs
./jsonTitle timing.json "50000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam100k.SAM $folder/100kOut.txt $folder/100kheaders.txt $folder/100kpositions.txt $runs
./jsonTitle timing.json "100000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam300k.SAM $folder/300kOut.txt $folder/300kheaders.txt $folder/300kpositions.txt $runs
./jsonTitle timing.json "300000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam500k.SAM $folder/500kOut.txt $folder/500kheaders.txt $folder/500kpositions.txt $runs
./jsonTitle timing.json "500000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam700k.SAM $folder/700kOut.txt $folder/700kheaders.txt $folder/700kpositions.txt $runs
./jsonTitle timing.json "700000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))

./samTo2Bit tests/samTests/sam1mil.SAM $folder/1milOut.txt $folder/1milheaders.txt $folder/1milpositions.txt $runs
./jsonTitle timing.json "1000000 bases"
#echo "" >> timing.json
cat timing.json >> $folder/sam2BTimeStats.json
((numInputFiles++))



#Add Title to sam2BTimeStats.txt file
./jsonTitle $folder/sam2BTimeStats.json "samTo2Bit" "-f"

#Add Final Title

#./jsonTitle timeStats.json "Final" "-f"

./timingJsonToCSV $folder/sam2BTimeStats.json $folder/sam2BTimeStats.csv $numInputFiles $runs $k


plotfile="sam2BTimeStats.csv"
output="samTo2Bit.png"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
graphTitle="Sam To 2Bit"
xlabel="Number of Bases"
xvals=1
yvals=2

gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath "$xlabel" $xvals $yvals




