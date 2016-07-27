#/bin/bash

gcc -g randFromKey.c -o randFromKey

numInputFiles=40

for ((i=1; i<(numInputFiles+1); ++i))
do
	./randFromKey tests/divAndOptTests/key"$i".txt 10000 tests/divAndOptTests/key"$i"In.txt
	
done
