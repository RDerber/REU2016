#/bin/bash

#Recompile all files before running timing function
gcc -g jsonTitle.c -o jsonTitle
gcc -g jsonData.c codonOptimizer.c -o codonOptimizer
gcc -g jsonData.c jsonSystemStats.c -o jsonSystemStats
gcc -g timingJsonToCSV.c -o timingJsonToCSV

if [ "$1" != ""  ]
then
	today=$1
else
	today=$(date +%Y-%m-%d-%T)
fi

#Make results diretory 
mkdir ./tests/codonTests/$today

#Adjustable timing parameters 
numInputFiles=0
runs=100
k=3
folder="./tests/codonTests/$today"

./jsonSystemStats $folder/codonTimeStats.json

#Run codonOptimizer Codon Tests and Store in timeStats.txt
./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta100.fasta $folder/100Out.txt $runs
./jsonTitle timing.json "100 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))
 # Add new line character inbetween files when appending

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta500.fasta $folder/500Out.txt $runs
./jsonTitle timing.json "500 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta1k.fasta $folder/1kOut.txt $runs
./jsonTitle timing.json "1000 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta5k.fasta $folder/5kOut.txt $runs
./jsonTitle timing.json "5000 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta10k.fasta $folder/10kOut.txt $runs
./jsonTitle timing.json "10000 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta50k.fasta $folder/50kOut.txt $runs
./jsonTitle timing.json "50000 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta100k.fasta $folder/100kOut.txt $runs
./jsonTitle timing.json "100000 bases" "-c"
echo "" >> timing.json
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))

./codonOptimizer codonAminoAcidKey.txt tests/codonTests/fasta300k.fasta $folder/300kOut.txt $runs
./jsonTitle timing.json "300000 bases" 
cat timing.json >> ./$folder/codonTimeStats.json
((numInputFiles++))



#Add Title to codonTimeStats.txt file
./jsonTitle ./$folder/codonTimeStats.json "codonOptimizer" "-f"

./timingJsonToCSV ./$folder/codonTimeStats.json ./$folder/codonTimeStats.csv $numInputFiles $runs $k

plotfile="codonTimeStats.csv"
output="codon.png"
graph="$folder/$plotfile"
touch $folder/$output
outpath="$folder/$output"
xlabel="Number of Codons"
graphTitle="Codon To Amino Acid Transformation"
xvals=1
yvals=2

gnuplot -c plotLineScript.sh $graph "$graphTitle" $outpath "$xlabel" $xvals $yvals




