/*
int addFunc(int a, int b);
*divideAndOptimize.h
*
*/

#ifndef DIVIDEANDOPTIMIZE_H
#define DIVIDEANDOPTIMIZE_H 


int andFunc(int a, int b);

int xorFunc(int a, int b);

int shiftLeftFunc(int a, int b);

int shiftRightFunc(int a, int b);

int assignFunc(int a, int b);

size_t highestBit(int num);

char findFirstDiff(unsigned char input, unsigned char* arr, size_t arrSize);

int findmin(unsigned char* input, int inputSize);

//char operate(unsigned char input, enum operationID opID, int val);
	
int compare (unsigned char* input, int inputSize, unsigned char * startingInput); 

int boolify(unsigned char a,unsigned char b, unsigned char *opsSeq,unsigned int *numSeq,int maxNumOps);
	

int superOptimizer (unsigned char * startingInput, unsigned char * input, int inputSize, int totOps, int numOps, char* opsSeq,unsigned int* numSeq);
	

int divideAndOptimize(unsigned char * startingInput, unsigned char* input, unsigned char *output, int inputSize,char** opArray,unsigned int** numArray, int maxNumOps, int opLevel);


#endif
