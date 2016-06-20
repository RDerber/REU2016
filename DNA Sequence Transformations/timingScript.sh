#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c FastaTo2Bit.c -o FastaTo2Bit
gcc writeJson.c MultiFastaTo4Bit.c -o MultiFastaTo4Bit 

#Remove old timeStats.txt files
rm timeStats.txt
rm ft2bTimeStats.txt
rm mt4bTimeStats.txt

#Run Fasta To 2 Bit Tests and Store in timeStats.txt
./FastaTo2Bit test1.txt 2Bit.txt 100 3
./jsonTitle timing.txt "test1" "-c"
echo "" >> timing.txt
cat timing.txt >> ft2bTimeStats.txt
 # Add new line character inbetween files when appending

./FastaTo2Bit test2.txt 2Bit.txt 100 3
./jsonTitle timing.txt "test2" "-c"
echo "" >> timing.txt
cat timing.txt >> ft2bTimeStats.txt

./FastaTo2Bit test3.txt 2Bit.txt 100 3
./jsonTitle timing.txt "test3"
cat timing.txt >> ft2bTimeStats.txt

#Add Title to ft2bTimeStats.txt file
./jsonTitle ft2bTimeStats.txt "FastaTo2Bit" "-t" 

#Run MultiFasta to 4Bit
./MultiFastaTo4Bit multiFasta 4Bit.txt 100 3
./jsonTitle timing.txt "multiFasta" 
cat timing.txt >> mt4bTimeStats.txt

#Add Title to mt4bTimeStats
./jsonTitle mt4bTimeStats.txt "MultiFastaTo4Bit" "-t"

echo "" >> ft2bTimeStats.txt
cat ft2bTimeStats.txt >> timeStats.txt
cat mt4bTimeStats.txt >> timeStats.txt

#Add Final Title

./jsonTitle timeStats.txt "Final" "-f"
