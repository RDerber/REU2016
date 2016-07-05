#/bin/bash

today=$(date +%Y-%m-%d-%T)
mkdir ./tests/benchmarkTests/$today

echo Fasta Timing
sh fastaScript.sh $today
cp ./tests/fastaTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/fastaTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/fastaTests/$today/*json ./tests/benchmarkTests/$today

echo Codon Timing
sh codonScript.sh $today
cp ./tests/codonTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/codonTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/codonTests/$today/*json ./tests/benchmarkTests/$today

echo Mutli-Fasta Timing
sh multiScript.sh $today
cp ./tests/multiTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/multiTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/multiTests/$today/*json ./tests/benchmarkTests/$today

echo Sam Timing
sh samScript.sh $today
cp ./tests/samTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/samTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/samTests/$today/*json ./tests/benchmarkTests/$today



