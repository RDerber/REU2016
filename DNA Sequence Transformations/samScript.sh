#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c SamTo2Bit.c -o SamTo2Bit

if [ $1 != NULL ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/samTests/$today

#Remove old timeStats.txt files
#rm timeStats.json
#rm sam2BTimeStats.json
#rm mt4bTimeStats.json

#Run SAM To 2 Bit Tests and Store in timeStats.txt
./SamTo2Bit tests/samTests/sam100.SAM tests/samTests/$today/100Out.txt tests/samTests/$today/100headers.txt tests/samTests/$today/100positions.txt 100 3
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json
 # Add new line character inbetween files when appending

./SamTo2Bit tests/samTests/sam500.SAM tests/samTests/$today/500Out.txt tests/samTests/$today/500headers.txt tests/samTests/$today/500positions.txt 100 3
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam1k.SAM tests/samTests/$today/1kOut.txt tests/samTests/$today/1kheaders.txt tests/samTests/$today/1kpositions.txt 100 3
./jsonTitle timing.json "1k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam5k.SAM tests/samTests/$today/5kOut.txt tests/samTests/$today/5kheaders.txt tests/samTests/$today/5kpositions.txt 100 3
./jsonTitle timing.json "5k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam10k.SAM tests/samTests/$today/10kOut.txt tests/samTests/$today/10kheaders.txt tests/samTests/$today/10kpositions.txt 100 3
./jsonTitle timing.json "10k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam50k.SAM tests/samTests/$today/50kOut.txt tests/samTests/$today/50kheaders.txt tests/samTests/$today/50kpositions.txt 100 3
./jsonTitle timing.json "50k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam100k.SAM tests/samTests/$today/100kOut.txt tests/samTests/$today/100kheaders.txt tests/samTests/$today/100kpositions.txt 100 3
./jsonTitle timing.json "100k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam500k.SAM tests/samTests/$today/500kOut.txt tests/samTests/$today/500kheaders.txt tests/samTests/$today/500kpositions.txt 100 3
./jsonTitle timing.json "500k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

./SamTo2Bit tests/samTests/sam1mil.SAM tests/samTests/$today/1milOut.txt tests/samTests/$today/1milheaders.txt tests/samTests/$today/1milpositions.txt 100 3
./jsonTitle timing.json "1mil bases"
echo "" >> timing.json
cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

#./SamTo2Bit tests/samTests/sam5mil.SAM tests/samTests/$today/5milOut.txt tests/samTests/$today/headers.txt tests/samTests/$today/100positions.txt 100 3
#./jsonTitle timing.json "5mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

#./SamTo2Bit tests/samTests/sam10mil.SAM tests/samTests/$today/10milOut.txt tests/samTests/$today/headers.txt tests/samTests/$today/100positions.txt 100 3
#./jsonTitle timing.json "10mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

#./SamTo2Bit tests/samTests/SAM15mil.SAM tests/samTests/$today/15milOut.txt tests/samTests/$today/headers.txt tests/samTests/$today/100positions.txt 100 3
#./jsonTitle timing.json "15mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

#./SamTo2Bit tests/samTests/SAM20mil.SAM tests/samTests/$today/20milOut.txt tests/samTests/$today/headers.txt tests/samTests/$today/100positions.txt 100 3
#./jsonTitle timing.json "20mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/samTests/$today/sam2BTimeStats.json

#Add Title to sam2BTimeStats.txt file
./jsonTitle ./tests/samTests/$today/sam2BTimeStats.json "SamTo2Bit"  

#Add Final Title

#./jsonTitle timeStats.json "Final" "-f"

./jsonToCSV ./tests/samTests/$today/sam2BTimeStats.json ./tests/samTests/$today/sam2BTimeStats.csv

plotfile="sam2BTimeStats.csv"
output="samTo2Bit.png"
folder="./tests/samTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"

gnuplot -c plotLineScript.sh $graph $output $outpath




