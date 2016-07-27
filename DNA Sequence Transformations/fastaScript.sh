#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc -g jsonData.c FastaTo2Bit.c -o FastaTo2Bit
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g timingJsonToCSV.c -o timingJsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/fastaTests/$today

numInputFiles=0
runs=100
k=3
folder="./tests/fastaTests/$today"

#Test Fasta for accuracy
./FastaTo2Bit tests/fastaTests/fastaTest.fasta $folder/testOut.txt 1

DIFF=$(diff -a tests/fastaTests/testCheck.fasta $folder/testOut.txt)
if [ "$DIFF" != "" ] 
then
	echo "The FASTA to 2Bit conversion is no longer correct. Terminating timing script."
	exit
fi

#Insert System Stats into output file
./jsonSystemStats $folder/ft2bTimeStats.json

#Run Fasta To 2 Bit Tests and Store in timeStats.txt
./FastaTo2Bit tests/fastaTests/fasta100.fasta $folder/100Out.txt $runs
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))
 # Add new line character inbetween files when appending

./FastaTo2Bit tests/fastaTests/fasta500.fasta tests/fastaTests/$today/500Out.txt $runs
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta1k.fasta tests/fastaTests/$today/1kOut.txt $runs
./jsonTitle timing.json "1000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta5k.fasta tests/fastaTests/$today/5kOut.txt $runs
./jsonTitle timing.json "5000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta10k.fasta tests/fastaTests/$today/10kOut.txt $runs
./jsonTitle timing.json "10000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta50k.fasta tests/fastaTests/$today/50kOut.txt $runs
./jsonTitle timing.json "50000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta100k.fasta tests/fastaTests/$today/100kOut.txt $runs
./jsonTitle timing.json "100000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta300k.fasta tests/fastaTests/$today/300kOut.txt $runs
./jsonTitle timing.json "300000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta500k.fasta tests/fastaTests/$today/500kOut.txt $runs
./jsonTitle timing.json "500000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta700k.fasta tests/fastaTests/$today/700kOut.txt $runs
./jsonTitle timing.json "700000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

./FastaTo2Bit tests/fastaTests/fasta1mil.fasta tests/fastaTests/$today/1milOut.txt $runs
./jsonTitle timing.json "1000000 bases"
cat timing.json >> $folder/ft2bTimeStats.json
((numInputFiles++))

#Add Title to ft2bTimeStats.txt file
./jsonTitle $folder/ft2bTimeStats.json "FastaTo2Bit" "-f"

./timingJsonToCSV $folder/ft2bTimeStats.json $folder/ft2bTimeStats.csv $numInputFiles $runs $k

plotfile="ft2bTimeStats.csv"
output="fastaTo2Bit.png"

graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
graphTitle="Fasta To 2Bit"
xlabel="Number of Bases"
xvals=1
yvals=2

gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath "$xlabel" $xvals $yvals




