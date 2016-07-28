# REU2016

GNU Plot Timing
===============
First and foremost this project is a benchmark suite of genomic sequencing conversions. It is intended to be used to test the efficiency of different architectures on these types of conversions. See the timing section below for more information on running timing tests. 

DNA Sequence Conversions
========================
These files are our own implementations of transformations from many different DNA Sequence storage formats to others (mostly a 2-bit format defined by us). We chose this 2 bit format as it is the minimal amount of space in which one can store all 4 different bases. The specific mapping is: A:00 C:01 G:11 T:10. This mapping allows us to use a very fast transformation from ascii to 2-bit by extracting the second and third lowest priority bits in the ascii character.

Generalized Mapping Algorithms
==============================
LaGrange Polynomial
-------------------
We did some exploration to create some more interesting and more generalized options of tranforming data. First we made a function that creates a LaGrange Polynomial mapping according to a key given to it. We attempted to optimize this algorithm using fast exponeniation as well as modding. The modding is inteded to keep the numbers used small for fast arithmetic as well as prevent overflow (with any number of inputs, even when only within the range of ascii values, our calculations quickly exceed that supported by precise enough C native types). To allow for the division involved in LaGrange polynomials we had to use a prime mod, as all numbers other than 0 in those sets have an inverse. We performed division by modular inverse. The speed of the modding itself was also a concern. To alleviate this we limited our possible set of mods to the mersenne primes, which are one less than a power of 2. This allowed us to use a special modding technique discribed here http://www.mersenneforum.org/showthread.php?t=1955. This technique sped up our evaluation of inputs by an average of 10%.

Super Optimizer
---------------
This function finds the shortest sequence of specified operations to map a set of n inputs to 0-(n-1). Its usefulness is questionable. For small sets of inputs, it quickly finds an extremely efficient method of tranforming data into an easily compressible state. It would be easy to change this function to transform data in other ways. The major issue, is the time that it takes to discover this sequence of operations. As we must test all possible combination, the time it takes to solve grows exponentially with the number of operations it takes to solve it. Furthermore, the number of operations it will take is highly variable depending on the input set and impractical to predict. In order to view the sequence found by superOptimizer, one must uncomment the last 12 lines of the main method in superOptimizer, which are responsible for printing out the sequence to the command line prompt. 

Benchmark Timing
=================
Each of the DNA sequence transforms as well as the mapping functions have timing structures built into them that allow the user to easily examine the time complexity of each program and montitor it while making changes to either the programs themselves or the architecture on which they are running. We have provided bash scripts for timing these functions individually and plotting the resultant data, as well as a benchmark script that when run, will execute all of the individual timing scripts and store the relevant JSON timing information files, csv files, and graphs for each program in one folder named with the current date and time. If an individual script is run, only that program will be timed, and its resultant timing information will be stored in the tests folder specific to that program, in a folder with the date and time that the script was executed. 

The following programs are tested using the same input sets each time, which were originaly generated randomly:
-fastaTo2Bit.c
-fastqTo2Bit.c
-multiFastaTo2Bit.c
-samTo2Bit.c
-codonOptimizer.c 

The following programs are tested using input/output keys of various sizes that are generated randomly each time the program is run:
-divideAndOptimize.c
-laGrangeGen.c

The following programs are tested using random inputs that are generated each time inside the function itself, as opposed to being passed in as arguments or input files: 
-superOptimizer.c 

A Note on Directory Layout
--------------------------
The timing scripts are currently designed to be run from the DNA Sequences directory along with the corresponding tests folder and the individual test folders inside it (fastaTests, samTests, etc.) that have randomly generated test data sets within them. Additonaly, all the C files and excecutables must be available in the same DNA Sequences Folder as the scripts. 
This directory layout is essential to the functionality of the timing scripts which reference these folders and executables while they run. If a different directtory layout is desired, changes will need to be made to each individual script so that the files referenced match up with their new locations. 

Randomly Generated Data Sets
----------------------------
For the timing of each program, the datasets used were created using the following random file generators: 

randFileGen.c - creates random FASTA, Multi-FASTA, FASTQ, or SAM input files 
keyGen.c - creates a random input/output key used to test divideAndOptimize.c or laGrangeGen.c
randFromKey.c - generates a random input file of specified size using the input characters of a key

