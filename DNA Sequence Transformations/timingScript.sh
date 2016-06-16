#/bin/bash

#Recompile FastaTo2Bit.c before running timing function
gcc writeJson.c FastaTo2Bit.c -o FastaTo2Bit
./FastaTo2Bit test1.txt 2Bit.c 100 3

#Remove old timeStats.txt file
rm timeStats.txt

#Run Tests and Store in timeStats.txt
./jsonTitle timing.txt "test1" "-c"
cat timing.txt >> timeStats.txt
./FastaTo2Bit test2.txt 2Bit.c 100 3
./jsonTitle timing.txt "test2" "-c"
cat timing.txt >> timeStats.txt
./FastaTo2Bit test3.txt 2Bit.c 100 3
./jsonTitle timing.txt "test3"
cat timing.txt >> timeStats.txt

#Add Title to timeStats.txt file
./jsonTitle timeStats.txt "FastaTo2Bit" "-f"
