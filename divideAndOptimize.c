/*
*divideAndOptimize.c
*
*Divides the original input array into smaller arrays using operations, splitting it in half each time, until final arrays of 2 are * *	*reached. Then the elements in the final arrays are mapped to the correct output groups.  
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum operationID {add = 0, and = 1, xor = 2, lsl = 3, lsr = 4};

int findFirstNot(unsigned char input, char arr[], size_t arrSize){
	int i;
	for(i = 0; i < arrSize; ++i){
		if(input != arr[i]) return i;
	}
	return -1;
}

int findmin(unsigned char* input, int inputSize){
	int min = input[0];
	int i;
	for(i = 1; i<inputSize; ++i){
		if(input[i] < min) min = input[i];
	}
	return min;
}

char operator(unsigned char input, enum operationID opID, int val){
	switch(opID){
		case add:
			input = input + val;
			break;

		case and:
			input = input & ~val;
			break;	

		case xor:
			input = input ^ val;
			break;

		case lsl:
			input = input << val;
			break;

		case lsr:
			input = input >> val;
			break;
	}
	return input;
}

int compare (unsigned char* input, int inputSize, unsigned char * startingInput){ // Checks to see if input values have been mapped
										// to 2 separate groups. Then returns the size of 
										// the first group. 
	int i,j;
	int group1 = input[0];
	int firstDiffIndex = findFirstNot(group1, input, inputSize);
	if(firstDiffIndex == -1) return -1;
	int group2 = input[firstDiffIndex];
	int count1 = firstDiffIndex;
	int count2 = 1;
	for(j = count1 + 1; j < inputSize; ++j){
		if(input[j] == group1)++count1;
		else if(input[j] != group2) return -1;
		else ++count2;
	}

	if(count1 >= inputSize/2 && count2 >= inputSize/2){
		printf("Transformations:\nInput:\tOutput:\n");
		for(i=0;i<inputSize;++i)
			printf("%d%s%d\n",startingInput[i],"   ->   ", input[i]);
		return count1;
	}else return -1;
}

int superOptimizer (unsigned char * startingInput, unsigned char * input, int inputSize, int totOps, int numOps, char* opsSeq, int* numSeq){
	int i,j,k;
	char newinput[inputSize];
	char operations[] = {'+','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int opsMax[] = {256, 256, 256, 8, 8};
	int group1Size = 0;
	for(k = 0; k < opsSize; ++k){
		int opIt = totOps - numOps;
		int constMax = opsMax[k];
		if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
					(k==3 && opsSeq[opIt-1] == operations[4]) ||
						(k==4 && opsSeq[opIt -1] == operations[3])))
							continue;
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < inputSize; ++j){				
				newinput[j] = operator(input[j],k,i);
				opsSeq[opIt] = operations[k];
				numSeq[opIt] = i;
			}
		if((group1Size=compare(newinput, inputSize,  startingInput))!= -1) return group1Size; // If successful sequence of 	
									//operations is found, return the size of the first group
		else if(numOps > 1 &&
			superOptimizer(startingInput, newinput, inputSize, totOps, numOps - 1, opsSeq, numSeq)!= -1)
				return 0; // Should never be returned if a match is found
		}
	}
	return -1;
}

int divideAndOptimize(unsigned char * startingInput, unsigned char* input, int inputSize, int maxNumOps, char* opsSeq, int * numSeq, int opLevel){
	// Note: startingInput and input start out as the same thing
//Base Case: Separated into arrays of size 2
	if(inputSize == 2){
		return 0; // Pass arrays into a separate function after this? 
	}

	int group1Size = superOptimizer(startingInput, input, inputSize, MaxNumOps, maxNumOps, opsSeq, numSeq);
	int group2Size = inputSize-group1Size;

//Separate inputs into groups based on the last superOptimizer run	
	char group1[group1size];
	char group2[group2size];
	int i;
	int g1counter = 0; // There may be a more efficient way of adding to 2 separate arrays
	int g2counter = 0;
	for(i=0; i<inputSize; ++i){ 			// Sorting out original values based on what they mapped to
		if(input[i]==0){			// **** Assuming that half of values will always map to 0. Is this true? 
							// Otherwise, the comparison value can be set to input[0] and start at i=1; 
			group1[g1counter++] = startingInput[i];
		}
		else{
			group2[g2counter++] = startingInput[i];
		}
	}


//Recover opsSeq and numSeq from the last superOptimizer run
	opArray[opLevel] = opsSeq; 				// NEEDS TO BE WORKED ON - Array of arrays of operations
	numArray[opLevel] = numSeq; 				// Need to define outside and pass in as parameter
	opLevel++;						// Need to determine how to differentiate entries from separate branches

// Fill in new array of opsSeq and numSeq
	char g1opsSeq [maxNumOps];
	int g1numSeq [maxNumOps];
	char g2opsSeq [maxNumOps];
	int g2numSeq [maxNumOps];
	for(i = 0; i < maxNumOps; ++i){
		g1opsSeq[i] = ' ';
		g1numSeq[i] = 0;
		g2opsSeq[i] = ' ';
		g2numSeq[i] = 0;
	}
	
	
// Send each group off onto separate branches with empty opsSeq and numSeq arrays to fill in
	divideAndOptimize(startingInput, group1, group1Size, maxNumOps, g1opsSeq, g1numSeq);
	divideAndOptimzie(startingInput, group2, group2Size, maxNumOps, g2opsSeq, g2numSeq);	
	
}  


int main(int argc, char** argv){
	int i; 
//	char input[] = {'\x00','\x01','\x02','\x03'};
//	unsigned char input[] = {'\x24','\x32','\xf7','\xc3','\x10','\x89','\xfd','\x78','\x98','\x36','\x65','\xdc','\xa4','\xb9','\xb1','\x9d'};
	char input[1000];
	int inputSize = sizeof(input)/sizeof(char);
	srand(time(NULL));
	for(i=0;i<inputSize;++i){
		input[i] = rand();
	}
	int maxNumOps = 6;
	char opsSeq [maxNumOps];
	int numSeq [maxNumOps];
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}

	int success = -1;								// NEED TO IMPLEMENT divideAndOptimize in main

	success = superOptimizer(input, input, inputSize, maxNumOps, maxNumOps, opsSeq, numSeq); // NEED TO CHANGE DEFINITION OF SUCCESS

	int numOps = 0;
	for(i = 0; i<maxNumOps; ++i){
		if(numSeq[i] != 0){
			++numOps;
		}
	}

	if(success == 0){
		printf("Sequence Found: \n");
		int i, j;
		printf("%s%d\n", "Number of Operations: ", numOps);
		for(i=0;i < numOps; ++i){
			printf("%d%c ",i,':');
			int opit = maxNumOps - numOps + i;
			if(opsSeq[opit] == '&') numSeq[opit] = ~numSeq[opit];
			printf("%c %d",opsSeq[opit], numSeq[opit]);
			printf("\n");
		}
	} else printf("no sequence found");
	return success;
}
