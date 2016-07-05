#/bin/bash

#Recompile all files before running timing function
gcc jsonTitle.c -o jsonTitle
gcc writeJson.c divideAndOptimize.c -o divideAndOptimize

if [ $1 != NULL ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

mkdir ./tests/divAndOptTests/$today

#Run divideAndOptimize Tests and Store in timeStats.txt
./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/100Out.txt 100 3
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json
 # Add new line character inbetween files when appending

./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/500Out.txt 100 3
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/1kOut.txt 100 3
./jsonTitle timing.json "1k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/5kOut.txt 100 3
./jsonTitle timing.json "5k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/10kOut.txt 100 3
./jsonTitle timing.json "10k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/50kOut.txt 100 3
./jsonTitle timing.json "50k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/100kOut.txt 100 3
./jsonTitle timing.json "100k bases" "-c"
echo "" >> timing.json
cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

#./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/500kOut.txt 100 3
#./jsonTitle timing.json "500k bases" "-c"
#echo "" >> timing.json
#cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

#./divideAndOptimize codonAminoAcidKey.txt tests/divAndOptTests/fasta50k.fasta tests/divAndOptTests/$today/1milOut.txt 100 3
#./jsonTitle timing.json "1mil bases"
#echo "" >> timing.json
#cat timing.json >> ./tests/divAndOptTests/$today/codonTimeStats.json

#Add Title to codonTimeStats.txt file
./jsonTitle ./tests/divAndOptTests/$today/codonTimeStats.json "divideAndOptimize codonAminoAcidKey.txt"  

./jsonToCSV ./tests/divAndOptTests/$today/codonTimeStats.json ./tests/divAndOptTests/$today/codonTimeStats.csv
plotfile="codonTimeStats.csv"
output="codon.png"
folder="./tests/divAndOptTests/$today"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of Codons"

gnuplot -c plotScript.sh $graph $output $outpath "$xlabel"




