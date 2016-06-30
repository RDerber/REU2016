#/bin/bash

fileType=$1

fileLabel=$2

fileSignature=$3

gcc randFileGen.c -o randFileGen

# Generation of random files, sizes 100, 500, 1k, 5k, 10k, 50k,100k,500k, 1mil and 5 mil characters 
./randFileGen $fileType 100 ./tests/${fileLabel}Tests/${fileLabel}100.$fileSignature 4

./randFileGen $fileType 500 ./tests/${fileLabel}Tests/${fileLabel}500.$fileSignature 4

./randFileGen $fileType 1000 ./tests/${fileLabel}Tests/${fileLabel}1k.$fileSignature 4

./randFileGen $fileType 5000 ./tests/${fileLabel}Tests/${fileLabel}5k.$fileSignature 4

./randFileGen $fileType 10000 ./tests/${fileLabel}Tests/${fileLabel}10k.$fileSignature 4

./randFileGen $fileType 50000 ./tests/${fileLabel}Tests/${fileLabel}50k.$fileSignature 4

./randFileGen $fileType 100000 ./tests/${fileLabel}Tests/${fileLabel}100k.$fileSignature 4

./randFileGen $fileType 500000 ./tests/${fileLabel}Tests/${fileLabel}500k.$fileSignature 4

./randFileGen $fileType 1000000 ./tests/${fileLabel}Tests/${fileLabel}1mil.$fileSignature 4

./randFileGen $fileType 5000000 ./tests/${fileLabel}Tests/${fileLabel}5mil.$fileSignature 4 





