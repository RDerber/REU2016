#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c codonOptimizer.c -o codonOptimizer

if [ $1 != NULL ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/codonTests/$today

#Run codonOptimizer Codon Tests and Store in timeStats.txt
./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta100.fasta tests/codonTests/$today/100Out.txt 100 3
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json
 # Add new line character inbetween files when appending

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta500.fasta tests/codonTests/$today/500Out.txt 100 3
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta1k.fasta tests/codonTests/$today/1kOut.txt 100 3
./jsonTitle timing.json "1k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta5k.fasta tests/codonTests/$today/5kOut.txt 100 3
./jsonTitle timing.json "5k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta10k.fasta tests/codonTests/$today/10kOut.txt 100 3
./jsonTitle timing.json "10k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta50k.fasta tests/codonTests/$today/50kOut.txt 100 3
./jsonTitle timing.json "50k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta100k.fasta tests/codonTests/$today/100kOut.txt 100 3
./jsonTitle timing.json "100k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

#./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta500k.fasta tests/codonTests/$today/500kOut.txt 100 3
#./jsonTitle timing.json "500k bases" "-c"
#echo "" >> timing.json
#cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

#./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta1mil.fasta tests/codonTests/$today/1milOut.txt 100 3
#./jsonTitle timing.json "1mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/codonTests/$today/codonTimeStats.json

#Add Title to codonTimeStats.txt file
./jsonTitle ./tests/codonTests/$today/codonTimeStats.json "codonOptimizer codonAminoAcidKey.txt"  

./jsonToCSV ./tests/codonTests/$today/codonTimeStats.json ./tests/codonTests/$today/codonTimeStats.csv
plotfile="codonTimeStats.csv"
output="codon.png"
folder="./tests/codonTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of Codons"

gnuplot -c plotLineScript.sh $graph $output $outpath "$xlabel"




