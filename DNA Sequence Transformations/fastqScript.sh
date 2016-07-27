#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc -g jsonData.c fastqTo2Bit.c -o fastqTo2Bit
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g timingJsonToCSV.c -o timingJsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

#Make results diretory 
mkdir ./tests/fastqTests/$today

folder="./tests/fastqTests/$today"

#Adjustable timing parameters 
numInputFiles=0 
runs=100
k=3

#Test Fasta for accuracy
./fastqTo2Bit tests/fastqTests/fastqTest.fastq $folder/testOut.txt $folder/testHead.txt $folder/testPosition.txt $folder/testQuality.txt  1

DIFF=$(diff -a tests/fastqTests/testCheck.txt $folder/testOut.txt)
if [ "$DIFF" != "" ] 
then
	echo "The FASTQ to 2Bit conversion is no longer correct. Terminating timing script."
	exit
fi

./jsonSystemStats $folder/ft2bTimeStats.json

#Run fastqTo 2 Bit Tests and Store in timeStats.txt
./fastqTo2Bit ./tests/fastqTests/fastq100.fastq $folder/100Out.txt $folder/100Head.txt $folder/100Position.txt $folder/100Quality.txt $runs
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))
 # Add new line character inbetween files when appending

./fastqTo2Bit ./tests/fastqTests/fastq500.fastq $folder/500Out.txt $folder/500Head.txt $folder/500Position.txt $folder/500Quality.txt $runs
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq1k.fastq $folder/1kOut.txt $folder/1kHead.txt $folder/1kPosition.txt $folder/1kQuality.txt $runs
./jsonTitle timing.json "1000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq5k.fastq $folder/5kOut.txt $folder/5kHead.txt $folder/5kPosition.txt $folder/5kQuality.txt $runs
./jsonTitle timing.json "5000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq10k.fastq $folder/10kOut.txt $folder/10kHead.txt $folder/10kPosition.txt $folder/10kQuality.txt $runs
./jsonTitle timing.json "10000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq50k.fastq $folder/50kOut.txt $folder/50kHead.txt $folder/50kPosition.txt $folder/50kQuality.txt $runs
./jsonTitle timing.json "50000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq100k.fastq $folder/100kOut.txt $folder/100kHead.txt $folder/100kPosition.txt $folder/100kQuality.txt $runs
./jsonTitle timing.json "100000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq300k.fastq $folder/300kOut.txt $folder/300kHead.txt $folder/300kPosition.txt $folder/300kQuality.txt $runs
./jsonTitle timing.json "300000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq500k.fastq $folder/500kOut.txt $folder/500kHead.txt $folder/500kPosition.txt $folder/500kQuality.txt $runs
./jsonTitle timing.json "500000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq700k.fastq $folder/700kOut.txt $folder/700kHead.txt $folder/700kPosition.txt $folder/700kQuality.txt $runs
./jsonTitle timing.json "700000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./fastqTo2Bit ./tests/fastqTests/fastq1mil.fastq $folder/1milOut.txt $folder/1milHead.txt $folder/1milPosition.txt $folder/1milQuality.txt $runs
./jsonTitle timing.json "1000000 bases"
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

#Add Title to ft2bTimeStats.txt file
./jsonTitle $folder/ft2bTimeStats.json "fastqTo2Bit" "-f"


./timingJsonToCSV $folder/ft2bTimeStats.json $folder/ft2bTimeStats.csv $numInputFiles $runs $k

plotfile="ft2bTimeStats.csv"
output="fastqTo2Bit.png"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
graphTitle="FastQ To 2Bit Transformation"
xlabel="Number of Bases"
xvals=1
yvals=2

gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath "$xlabel" $xvals $yvals




