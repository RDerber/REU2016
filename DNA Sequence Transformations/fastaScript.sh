#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc jsonData.c FastaTo2Bit.c -o FastaTo2Bit
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g timingJsonToCSV.c -o timingJsonToCSV

if [ $1 != NULL ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/fastaTests/$today

numInputFiles=0
runs=100
k=3
#Remove old timeStats.txt files
#rm timeStats.json
#rm ft2bTimeStats.json
#rm mt4bTimeStats.json

#Run Fasta To 2 Bit Tests and Store in timeStats.txt
./FastaTo2Bit tests/fastaTests/fasta100.fasta tests/fastaTests/$today/100Out.txt $runs $k
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1
 # Add new line character inbetween files when appending

./FastaTo2Bit tests/fastaTests/fasta500.fasta tests/fastaTests/$today/500Out.txt $runs $k
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta1k.fasta tests/fastaTests/$today/1kOut.txt $runs $k
./jsonTitle timing.json "1k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta5k.fasta tests/fastaTests/$today/5kOut.txt $runs $k
./jsonTitle timing.json "5k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta10k.fasta tests/fastaTests/$today/10kOut.txt $runs $k
./jsonTitle timing.json "10k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta50k.fasta tests/fastaTests/$today/50kOut.txt $runs $k
./jsonTitle timing.json "50k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta100k.fasta tests/fastaTests/$today/100kOut.txt $runs $k
./jsonTitle timing.json "100k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta300k.fasta tests/fastaTests/$today/300kOut.txt $runs $k
./jsonTitle timing.json "300k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta500k.fasta tests/fastaTests/$today/500kOut.txt $runs $k
./jsonTitle timing.json "500k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta700k.fasta tests/fastaTests/$today/700kOut.txt $runs $k
./jsonTitle timing.json "700k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

./FastaTo2Bit tests/fastaTests/fasta1mil.fasta tests/fastaTests/$today/1milOut.txt $runs $k
./jsonTitle timing.json "1mil bases"
cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json
numInputFiles=numInputFiles+1

#./FastaTo2Bit tests/fastaTests/fasta5mil.fasta tests/fastaTests/$today/5milOut.txt $runs $k
#./jsonTitle timing.json "5mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fastaTests/fasta10mil.fasta tests/fastaTests/$today/10milOut.txt $runs $k
#./jsonTitle timing.json "10mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fastaTests/fasta15mil.fasta tests/fastaTests/$today/15milOut.txt $runs $k
#./jsonTitle timing.json "15mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fastaTests/fasta20mil.fasta tests/fastaTests/$today/20milOut.txt $runs $k
#./jsonTitle timing.json "20mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/fastaTests/$today/ft2bTimeStats.json

#Add Title to ft2bTimeStats.txt file
./jsonTitle ./tests/fastaTests/$today/ft2bTimeStats.json "FastaTo2Bit" 

#Run MultiFasta to 4Bit
#./MultiFastaTo4Bit multiFasta 4Bit.txt $runs $k
#./jsonTitle timing.json "multiFasta" 
#cat timing.json >> mt4bTimeStats.json

#Add Title to mt4bTimeStats
#./jsonTitle mt4bTimeStats.json "MultiFastaTo4Bit" "-t"

#echo "" >> ft2bTimeStats.json
#cat ft2bTimeStats.json >> timeStats.json
#cat mt4bTimeStats.json >> timeStats.json

#Add Final Title

#./jsonTitle timeStats.json "Final" "-f"

./timingJsonToCSV ./tests/fastaTests/$today/ft2bTimeStats.json ./tests/fastaTests/$today/ft2bTimeStats.csv numInputFiles $runs $k

plotfile="ft2bTimeStats.csv"
output="fastaTo2Bit.png"
folder="./tests/fastaTests/$today"
graph="$folder/$plotfile"
#touch $folder/$output
outpath="$folder/$output"
graphTitle="Fasta To 2Bit"

#gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath




