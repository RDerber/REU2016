#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c MultiFastaTo2Bit.c -o MultiFastaTo2Bit
# gcc writeJson.c MultiFastaTo4Bit.c -o MultiFastaTo4Bit # 4Bit currently not used

if [ $1 != NULL ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/multiTests/$today


#Remove old timeStats.txt files
#rm timeStats.json
#rm mt2bTimeStats.json
#rm mt4bTimeStats.json

#Run multiFastaTo 2 Bit Tests and Store in timeStats.txt
./MultiFastaTo2Bit tests/multiTests/multi100.fasta tests/multiTests/$today/100Out.txt tests/multiTests/$today/100Head.txt tests/multiTests/$today/100Position.txt 100 3
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json
 # Add new line character inbetween files when appending

./MultiFastaTo2Bit tests/multiTests/multi500.fasta tests/multiTests/$today/500Out.txt tests/multiTests/$today/500Head.txt tests/multiTests/$today/500Position.txt 100 3
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi1k.fasta tests/multiTests/$today/1kOut.txt tests/multiTests/$today/1kHead.txt tests/multiTests/$today/1kPosition.txt 100 3
./jsonTitle timing.json "1k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi5k.fasta tests/multiTests/$today/5kOut.txt tests/multiTests/$today/5kHead.txt tests/multiTests/$today/5kPosition.txt 100 3
./jsonTitle timing.json "5k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi10k.fasta tests/multiTests/$today/10kOut.txt tests/multiTests/$today/10kHead.txt tests/multiTests/$today/10kPosition.txt 100 3
./jsonTitle timing.json "10k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi50k.fasta tests/multiTests/$today/50kOut.txt tests/multiTests/$today/50kHead.txt tests/multiTests/$today/50kPosition.txt 100 3
./jsonTitle timing.json "50k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi100k.fasta tests/multiTests/$today/100kOut.txt tests/multiTests/$today/100kHead.txt tests/multiTests/$today/100kPosition.txt 100 3
./jsonTitle timing.json "100k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi300k.fasta tests/multiTests/$today/300kOut.txt tests/multiTests/$today/300kHead.txt tests/multiTests/$today/100kPosition.txt 100 3
./jsonTitle timing.json "300k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi500k.fasta tests/multiTests/$today/500kOut.txt tests/multiTests/$today/500kHead.txt tests/multiTests/$today/500kPosition.txt 100 3
./jsonTitle timing.json "500k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi700k.fasta tests/multiTests/$today/700kOut.txt tests/multiTests/$today/700kHead.txt tests/multiTests/$today/100kPosition.txt 700 3
./jsonTitle timing.json "700k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

./MultiFastaTo2Bit tests/multiTests/multi1mil.fasta tests/multiTests/$today/1milOut.txt tests/multiTests/$today/1milHead.txt tests/multiTests/$today/1milPosition.txt 100 3
./jsonTitle timing.json "1mil bases"
echo "" >> timing.json
cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi5mil.fasta tests/multiTests/$today/5milOut.txt 100 3
#./jsonTitle timing.json "5mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi10mil.fasta tests/multiTests/$today/10milOut.txt 100 3
#./jsonTitle timing.json "10mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi15mil.multi tests/multiTests/$today/15milOut.txt 100 3
#./jsonTitle timing.json "15mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

#./MultiFastaTo2Bit tests/multiTests/multi20mil.fasta tests/multiTests/$today/20milOut.txt 100 3
#./jsonTitle timing.json "20mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/multiTests/$today/mt2bTimeStats.json

#Add Title to mt2bTimeStats.txt file
./jsonTitle ./tests/multiTests/$today/mt2bTimeStats.json "MultiFastaTo2Bit"  

#Run MultiFasta to 4Bit
#./MultiFastaTo4Bit multiFasta 4Bit.txt 100 3
#./jsonTitle timing.json "multiFasta" 
#cat timing.json >> mt4bTimeStats.json

#Add Title to mt4bTimeStats
#./jsonTitle mt4bTimeStats.json "MultiFastaTo4Bit" "-t"

#echo "" >> mt2bTimeStats.json
#cat mt2bTimeStats.json >> timeStats.json
#cat mt4bTimeStats.json >> timeStats.json

#Add Final Title

#./jsonTitle timeStats.json "Final" "-f"

./jsonToCSV ./tests/multiTests/$today/mt2bTimeStats.json ./tests/multiTests/$today/mt2bTimeStats.csv

plotfile="mt2bTimeStats.csv"
output="MultiFastaTo2Bit.png"
folder="./tests/multiTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
graphTitle="MultiFasta To 2Bit"
gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath




