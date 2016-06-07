# REU2016
DNA Sequence Conversions

LaGrangeGen.c
Input: [inputfile] [outputfile] [Any number of ascii characters] [Y values corresponding to each character***]

***No Y-Value can exceed 8190. Otherwise, it will be modded in the creation of the polynomial and it will become a smaller number. Larger Y-values can be used if a mersenne prime that is larger than any y-value is added to the mersenne prime array in the findMod method. In addition you must also alter the initial value that mod is set to in polyGenerator to the new product of all of the mersenne primes in the mersenne array. Its current value, using the first four mersenne primes in the array, is 225735769.
