#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc jsonData.c MultiFastaTo2Bit.c -o MultiFastaTo2Bit
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g timingJsonToCSV.c -o timingJsonToCSV

if [ "$1" != "" ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/multiTests/$today

folder="./tests/multiTests/$today"

numInputFiles=0 
runs=100
k=3

./jsonSystemStats $folder/ft2bTimeStats.json

#Run multiFastaTo 2 Bit Tests and Store in timeStats.txt
./MultiFastaTo2Bit ./tests/multiTests/multi100.fasta $folder/100Out.txt $folder/100Head.txt $folder/100Position.txt $runs
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))
 # Add new line character inbetween files when appending

./MultiFastaTo2Bit ./tests/multiTests/multi500.fasta $folder/500Out.txt $folder/500Head.txt $folder/500Position.txt $runs
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi1k.fasta $folder/1kOut.txt $folder/1kHead.txt $folder/1kPosition.txt $runs
./jsonTitle timing.json "1000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi5k.fasta $folder/5kOut.txt $folder/5kHead.txt $folder/5kPosition.txt $runs
./jsonTitle timing.json "5000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi10k.fasta $folder/10kOut.txt $folder/10kHead.txt $folder/10kPosition.txt $runs
./jsonTitle timing.json "10000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi50k.fasta $folder/50kOut.txt $folder/50kHead.txt $folder/50kPosition.txt $runs
./jsonTitle timing.json "50000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi100k.fasta $folder/100kOut.txt $folder/100kHead.txt $folder/100kPosition.txt $runs
./jsonTitle timing.json "100000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi300k.fasta $folder/300kOut.txt $folder/300kHead.txt $folder/100kPosition.txt $runs
./jsonTitle timing.json "300000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi500k.fasta $folder/500kOut.txt $folder/500kHead.txt $folder/500kPosition.txt $runs
./jsonTitle timing.json "500000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi700k.fasta $folder/700kOut.txt $folder/700kHead.txt $folder/100kPosition.txt $runs
./jsonTitle timing.json "700000 bases" "-c"
echo "" >> timing.json
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

./MultiFastaTo2Bit ./tests/multiTests/multi1mil.fasta $folder/1milOut.txt $folder/1milHead.txt $folder/1milPosition.txt $runs
./jsonTitle timing.json "1000000 bases"
cat timing.json >> $folder/mt2bTimeStats.json
((numInputFiles++))

#./MultiFastaTo2Bit tests/multiTests/multi5mil.fasta $folder/5milOut.txt $runs
#./jsonTitle timing.json "5mil bases"
#echo "" >> timing.json
#cat timing.json >> $folder/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi10mil.fasta $folder/10milOut.txt $runs
#./jsonTitle timing.json "10mil bases"
#echo "" >> timing.json
#cat timing.json >> $folder/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi15mil.multi $folder/15milOut.txt $runs
#./jsonTitle timing.json "15mil bases"
#echo "" >> timing.json
#cat timing.json >> $folder/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi20mil.fasta $folder/20milOut.txt $runs
#./jsonTitle timing.json "20mil bases"
#echo "" >> timing.json
#cat timing.json >> $folder/mt2bTimeStats.json

#Add Title to mt2bTimeStats.txt file
./jsonTitle $folder/mt2bTimeStats.json "MultiFastaTo2Bit" "-f"

#Run MultiFasta to 4Bit
#./MultiFastaTo4Bit multiFasta 4Bit.txt $runs
#./jsonTitle timing.json "multiFasta" 
#cat timing.json >> mt4bTimeStats.json

#Add Title to mt4bTimeStats
#./jsonTitle mt4bTimeStats.json "MultiFastaTo4Bit" "-t"

#echo "" >> mt2bTimeStats.json
#cat mt2bTimeStats.json >> timeStats.json
#cat mt4bTimeStats.json >> timeStats.json

#Add Final Title

#./jsonTitle timeStats.json "Final" "-f"

./timingJsonToCSV $folder/mt2bTimeStats.json $folder/mt2bTimeStats.csv $numInputFiles $runs $k

plotfile="mt2bTimeStats.csv"
output="MultiFastaTo2Bit.png"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
graphTitle="MultiFasta To 2Bit"
xlabel="Number of Bases"
xvals=1
yvals=2

gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath "$xlabel" $xvals $yvals




