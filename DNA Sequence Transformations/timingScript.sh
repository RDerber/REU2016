#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c FastaTo2Bit.c -o FastaTo2Bit
gcc writeJson.c MultiFastaTo4Bit.c -o MultiFastaTo4Bit 

#Remove old timeStats.txt files
rm timeStats.json
rm ft2bTimeStats.json
rm mt4bTimeStats.json

#Run Fasta To 2 Bit Tests and Store in timeStats.txt
./FastaTo2Bit test1.txt 2Bit.txt 100 3
./jsonTitle timing.json "test1" "-c"
echo "" >> timing.json
cat timing.json >> ft2bTimeStats.json
 # Add new line character inbetween files when appending

./FastaTo2Bit test2.txt 2Bit.txt 100 3
./jsonTitle timing.json "test2" "-c"
echo "" >> timing.json
cat timing.json >> ft2bTimeStats.json

./FastaTo2Bit test3.txt 2Bit.txt 100 3
./jsonTitle timing.json "test3"
cat timing.json >> ft2bTimeStats.json

#Add Title to ft2bTimeStats.txt file
./jsonTitle ft2bTimeStats.json "FastaTo2Bit" "-c" 

#Run MultiFasta to 4Bit
./MultiFastaTo4Bit multiFasta 4Bit.txt 100 3
./jsonTitle timing.json "multiFasta" 
cat timing.json >> mt4bTimeStats.json

#Add Title to mt4bTimeStats
./jsonTitle mt4bTimeStats.json "MultiFastaTo4Bit" "-t"

echo "" >> ft2bTimeStats.json
cat ft2bTimeStats.json >> timeStats.json
cat mt4bTimeStats.json >> timeStats.json

#Add Final Title

./jsonTitle timeStats.json "Final" "-f"
