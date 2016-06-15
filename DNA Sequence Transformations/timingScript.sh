#/bin/bash
gcc writeJson.c FastaTo2Bit.c -o FastaTo2Bit
./FastaTo2Bit test1.txt 2Bit.c 20 3
rm timeStats.txt
./jsonTitle timing.txt "test1" "-c"
cat timing.txt >> timeStats.txt
./FastaTo2Bit test2.txt 2Bit.c 20 3
./jsonTitle timing.txt "test2" "-c"
cat timing.txt >> timeStats.txt
./FastaTo2Bit test3.txt 2Bit.c 20 3
./jsonTitle timing.txt "test3"
cat timing.txt >> timeStats.txt
./jsonTitle timeStats.txt "FastaTo2Bit" "-f"
