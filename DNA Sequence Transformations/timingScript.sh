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
cat timing.txt >> ft2bTimeStats.txt
./FastaTo2Bit test2.txt 2Bit.txt 100 3
./jsonTitle timing.txt "test2" "-c"
cat timing.txt >> ft2bTimeStats.txt
./FastaTo2Bit test3.txt 2Bit.txt 100 3
./jsonTitle timing.txt "test3"
cat timing.txt >> ft2bTimeStats.txt

#Add Title to ft2bTimeStats.txt file
./jsonTitle ft2btimeStats.txt "FastaTo2Bit" "-t"

#Run MultiFasta to 4Bit
./MultiFastaTo4Bit multiFasta 4Bit.txt
./jsonTitle timing.txt "multiFasta" 100 3
cat timing.txt >> mt4bTimeStats.txt

#Add Title to mt4bTimeStats
./jsonTitle ft4bTimeStats.txt "MultiFastaTo4Bit" "-t"

cat ft2bTimeStats.txt >> timeStats.txt
cat mt4bTimeStats.txt >> timeStats.txt
