#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c FastaTo2Bit.c -o FastaTo2Bit
gcc writeJson.c MultiFastaTo4Bit.c -o MultiFastaTo4Bit 

today=$(date +%Y-%m-%d-%T)
mkdir ./tests/$today


#Remove old timeStats.txt files
#rm timeStats.json
#rm ft2bTimeStats.json
#rm mt4bTimeStats.json

#Run Fasta To 2 Bit Tests and Store in timeStats.txt
./FastaTo2Bit tests/fasta100.fasta tests/$today/100Out.txt 100 3
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json
 # Add new line character inbetween files when appending

./FastaTo2Bit tests/fasta500.fasta tests/$today/500Out.txt 100 3
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta1k.fasta tests/$today/1kOut.txt 100 3
./jsonTitle timing.json "1k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta5k.fasta tests/$today/5kOut.txt 100 3
./jsonTitle timing.json "5k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta10k.fasta tests/$today/10kOut.txt 100 3
./jsonTitle timing.json "10k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta50k.fasta tests/$today/50kOut.txt 100 3
./jsonTitle timing.json "50k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta100k.fasta tests/$today/100kOut.txt 100 3
./jsonTitle timing.json "100k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta500k.fasta tests/$today/500kOut.txt 100 3
./jsonTitle timing.json "500k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

./FastaTo2Bit tests/fasta1mil.fasta tests/$today/1milOut.txt 100 3
./jsonTitle timing.json "1mil bases"
echo "" >> timing.json
cat timing.json >> ./tests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fasta5mil.fasta tests/$today/5milOut.txt 100 3
#./jsonTitle timing.json "5mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fasta10mil.fasta tests/$today/10milOut.txt 100 3
#./jsonTitle timing.json "10mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fasta15mil.fasta tests/$today/15milOut.txt 100 3
#./jsonTitle timing.json "15mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/$today/ft2bTimeStats.json

#./FastaTo2Bit tests/fasta20mil.fasta tests/$today/20milOut.txt 100 3
#./jsonTitle timing.json "20mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/$today/ft2bTimeStats.json

#Add Title to ft2bTimeStats.txt file
./jsonTitle ./tests/$today/ft2bTimeStats.json "FastaTo2Bit"  

#Run MultiFasta to 4Bit
#./MultiFastaTo4Bit multiFasta 4Bit.txt 100 3
#./jsonTitle timing.json "multiFasta" 
#cat timing.json >> mt4bTimeStats.json

#Add Title to mt4bTimeStats
#./jsonTitle mt4bTimeStats.json "MultiFastaTo4Bit" "-t"

#echo "" >> ft2bTimeStats.json
#cat ft2bTimeStats.json >> timeStats.json
#cat mt4bTimeStats.json >> timeStats.json

#Add Final Title

#./jsonTitle timeStats.json "Final" "-f"

./jsonToCSV ./tests/$today/ft2bTimeStats.json ./tests/$today/ft2bTimeStats.csv

plotfile="ft2bTimeStats.csv"
output="fastaTo2Bit.png"
folder="./tests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"

gnuplot -c plotScript.sh $graph $output $outpath




