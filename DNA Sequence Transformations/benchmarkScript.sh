#/bin/bash
#Creates a benchmark report of the following programs:
# -FastaTo2Bit.c
# -MultiFastaTo2Bit.c
# -SamTo2Bit.c
# -CodonOptimizer.c
# -divideAndOptimize.c
#
# The benchmark consists of a directory titled with the current date, containing subdirectories with the following information for each function:
#   - A .json output file with all timing information
#   - Relative .csv files for each function 
#   - Relative .png graphs for each function
#

today=$(date +%Y-%m-%d-%T)
mkdir ./tests/benchmarkTests/$today


#DNA File Format Transformations
echo Fasta Timing
sh fastaScript.sh $today
cp ./tests/fastaTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/fastaTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/fastaTests/$today/*json ./tests/benchmarkTests/$today

echo Fastq Timing
sh fastqScript.sh $today
cp ./tests/fastqTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/fastqTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/fastqTests/$today/*json ./tests/benchmarkTests/$today

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

#SuperOptimization

#Mapping DNA Codons to Amino Acids 
echo Codon Timing
sh codonScript.sh $today
cp ./tests/codonTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/codonTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/codonTests/$today/*json ./tests/benchmarkTests/$today

#Mapping inputs to outputs using a key
echo Divide And Optimize 
sh divAndOptScript.sh $today
cp ./tests/divAndOptTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/divAndOptTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/divAndOptTests/$today/*json ./tests/benchmarkTests/$today


#Mapping n inputs to output values [0-n]

echo SuperOptimizer
sh superScript.sh $today
cp ./tests/superTests/$today/*png ./tests/benchmarkTests/$today
cp ./tests/superTests/$today/*csv ./tests/benchmarkTests/$today
cp ./tests/superTests/$today/*json ./tests/benchmarkTests/$today


