# REU2016

GNU Plot Timing
===============
First and foremost this project is a benchmark suite of genomic sequencing conversions. It is intended to be used to test the efficiency of different architectures on these types of conversions.

DNA Sequence Conversions
========================
These files are our own implementations of transformations from many different DNA Sequence storage formats to others (mostly a 2=bit format defined by us). We chose this 2 bit format as it is the minimal amount of space in which one can store all 4 different bases. The specific mapping is: A:00 C:01 G:11 T:10. This mapping allows us to use a very fast transformation from ascii to 2=bit by extracting the second and third lowest priority bits in the ascii character.

Generalized Mapping Algorithms
==============================
LaGrange Polynomial
-------------------
We did some exploration to create some more interesting and more generalized options of tranforming data. First we made a function that creates a LaGrange Polynomial mapping according to a key given to it. We attempted to optimize this algorithm using fast exponeniation as well as modding. The modding is inteded to keep the numbers used small for fast arithmetic as well as prevent overflow (with any number of inputs, even when only within the range of ascii values, our calculations quickly exceed that supported by precise enough C native types). To allow for the division involved in LaGrange polynomials we had to use a prime mod, as all numbers other than 0 in those sets have an inverse. We performed division by modular inverse. The speed of the modding itself was also a concern. To alleviate this we limited our possible set of mods to the mersenne primes, which are one less than a power of 2. This allowed us to use a special modding technique discribed here http://www.mersenneforum.org/showthread.php?t=1955. This technique sped up our evaluation of inputs by an average of 10%.

Super Optimizer
---------------
This function finds the shortest sequence of specified operations to map a set of n inputs to 0-(n-1). Its usefulness is questionable. For small sets of inputs, it quickly finds an extremely efficient method of tranforming data into an easily compressible state. It would be easy to change this function to transform data in other ways. The major issue, is the time that it takes to discover this sequence of operations. As we must test all possible combination, the time it takes to solve grows exponentially with the number of operations it takes to solve it. Furthermore, the number of operations it will take is highly variable depending on the input set and impractical to predict.
