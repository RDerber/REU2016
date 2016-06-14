#/bin/bash
gcc writeJson.c FastaTo2Bit.c -o FastaTo2Bit
./FastaTo2Bit test1.txt 2Bit.c 20 3
rm timeStats.txt
cat timing.txt >> timeStats.txt
./FastaTo2Bit test2.txt 2Bit.c 20 3
cat timing.txt >> timeStats.txt
./FastaTo2Bit test3.txt 2Bit.c 20 3
cat timing.txt >> timeStats.txt
