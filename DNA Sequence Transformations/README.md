# REU2016
DNA Sequence Conversions
========================
These files are our own implementations of transformations from many different DNA Sequence storage formats to others (mostly a 2-bit format defined by us). We chose this 2 bit format as it is the minimal amount of space in which one can store all 4 different bases. The specific mapping is: A:00 C:01 G:11 T:10. This mapping allows us to use a very fast transformation from ascii to 2-bit by extracting the second and third lowest priority bits in the ascii character.
