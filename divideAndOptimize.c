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

size_t highestBit(int num){
	int i;
	size_t check = 1;
	for(i=0;i < sizeof(int)*8;++i){
		check*=2;
	}
	for(i = sizeof(int) * 8;i > 0;--i){
		if(num&check) return check;
		else check/=2;
	}
}

char findFirstNot(unsigned char input, unsigned char* arr, size_t arrSize){
	int i;
	for(i = 0; i < arrSize; ++i){
		if(input != arr[i]) return i;
	}
	return 0;
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
										// to 2 separate groups. Then returns the size of 											// the first group. 
	
	int i,j;
	char group1 = input[0];
	
	char firstDiffIndex = findFirstNot(group1, input, inputSize);
	if(firstDiffIndex == 0){
		return 0;
	}
	char group2 = input[firstDiffIndex];
	int count1 = firstDiffIndex;
	int count2 = 1;
	for(j = count1 + 1; j < inputSize; ++j){
		if(input[j] == group1)++count1;
		else if(input[j] != group2){			 
			return 0;
			}
		else ++count2;
	}

	if(count1 >= inputSize/2 && count2 >= inputSize/2){
		printf("Transformations:\nInput:\tOutput:\n");
		for(i=0;i<inputSize;++i)
			printf("%d%s%d\n",startingInput[i],"   ->   ", input[i]);
		return count1;
	}else return 0;
}

int superOptimizer (unsigned char * startingInput, unsigned char * input, int inputSize, int totOps, int numOps, char* opsSeq,unsigned int* numSeq){
	int i,j,k;
	char constInput[inputSize];
	for(i=0;i<inputSize;++i) constInput[i] = input[i];
	char operations[] = {'+','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int opsMax[] = {256, 256, 256, 8, 8};
	int group1Size = 0;
	for(k = 0; k < opsSize; ++k){
		int opIt = totOps - numOps;
		int constMax = opsMax[k];
	//	if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
	//				(k==3 && opsSeq[opIt-1] == operations[4]) ||
	//					(k==4 && opsSeq[opIt -1] == operations[3])))
	//						continue;
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < inputSize; ++j){				
				input[j] = operator(constInput[j],k,i);
				opsSeq[opIt] = operations[k];
				numSeq[opIt] = i;
			}
			if((group1Size=compare(input, inputSize,  startingInput))!= 0){
				return group1Size; 	// If successful sequence of 	
			}				//operations is found, return the size of the first group
			else if(numOps > 1 &&
				(group1Size = superOptimizer(startingInput, input, 
						inputSize, totOps, numOps - 1, opsSeq, numSeq))!= -1){

				return group1Size;
			}
		}
	}
	return -1;
}

int finalOptimizer (unsigned char * startingInput, unsigned char * input, int inputSize, int totOps, int numOps, char* opsSeq,unsigned int* numSeq){
	int i,j,k;
	char constInput[inputSize];
	for(i=0;i<inputSize;++i) constInput[i] = input[i];
	char operations[] = {'+','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int opsMax[] = {256, 256, 256, 8, 8};
	int group1Size = 0;
	for(k = 0; k < opsSize; ++k){
		int opIt = totOps - numOps;
		int constMax = opsMax[k];
	//	if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
	//				(k==3 && opsSeq[opIt-1] == operations[4]) ||
	//					(k==4 && opsSeq[opIt -1] == operations[3])))
	//						continue;
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < inputSize; ++j){				
				input[j] = operator(constInput[j],k,i);
				opsSeq[opIt] = operations[k];
				numSeq[opIt] = i;
			}
			if((group1Size=finalCompare(input, inputSize,  startingInput))!= 0){
				return group1Size; 	// If successful sequence of 	
			}				//operations is found, return the size of the first group
			else if(numOps > 1 &&
				(group1Size = superOptimizer(startingInput, input, 
						inputSize, totOps, numOps - 1, opsSeq, numSeq))!= -1){

				return group1Size;
			}
		}
	}
	return -1;
}

int compare (unsigned char* input, int inputSize, unsigned char * startingInput){ // Checks to see if input values have been mapped
										// to 2 separate groups. Then returns the size of 											// the first group. 
	
	int i,j;
	char group1 = input[0];
	
	char firstDiffIndex = findFirstNot(group1, input, inputSize);
	if(firstDiffIndex == 0){
		return 0;
	}
	char group2 = input[firstDiffIndex];
	int count1 = firstDiffIndex;
	int count2 = 1;
	for(j = count1 + 1; j < inputSize; ++j){
		if(input[j] == group1)++count1;
		else if(input[j] != group2){			 
			return 0;
			}
		else ++count2;
	}

	if(count1 >= inputSize/2 && count2 >= inputSize/2){
		printf("Transformations:\nInput:\tOutput:\n");
		for(i=0;i<inputSize;++i)
			printf("%d%s%d\n",startingInput[i],"   ->   ", input[i]);
		return count1;
	}else return 0;
}

int divideAndOptimize(unsigned char * startingInput, unsigned char* input, int inputSize,char** opArray,unsigned int** numArray, int maxNumOps, int opLevel){
	// Note: startingInput and input start out as the same thing
//Base Case: Separated into arrays of size 2
	int i;
	if(inputSize <= 2){
		return 0; // Pass arrays into a separate function after this? 
	}

	char opsSeq[maxNumOps];
	unsigned int numSeq[maxNumOps];
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}
	int group1Size = superOptimizer(startingInput, input, inputSize, maxNumOps, maxNumOps, opsSeq, numSeq);
	if(group1Size == -1) return -1;
	int group2Size = inputSize-group1Size;

//Separate inputs into groups based on the last superOptimizer run	
	unsigned char group1[group1Size];
	unsigned char group2[group2Size];
	int g1counter = 0; // There may be a more efficient way of adding to 2 separate arrays
	int g2counter = 0;
	int groupCheck = input[0];
	for(i=0; i<inputSize; ++i){ 			// Sorting out original values based on what they mapped to
		if(input[i]==groupCheck){		// **** Assuming that half of values will always map to 0. Is this true? 
							// Otherwise, the comparison value can be set to input[0] and start at i=1; 
			group1[g1counter++] = startingInput[i];
		}
		else{
			group2[g2counter++] = startingInput[i];
		}
	}

//Recover opsSeq and numSeq from the last superOptimizer run
	for(i=0;i<maxNumOps;++i){
		opArray[opLevel][i] = opsSeq[i];
		numArray[opLevel][i] = numSeq[i];
	}

// Send each group off onto separate branches with empty opsSeq and numSeq arrays to fill in
	printf("%s%d\n","group1Size: ",group1Size);
	printf("%s%d\n","group2Size: ",group2Size);
	for(i=0; i<group1Size; ++i){
		printf("%s %d\n","group1:", group1[i]);
	}
	for(i=0; i<group2Size; ++i){
		printf("%s %d\n","group2:", group2[i]);
	}

	unsigned char startingGroup1[group1Size];
	unsigned char startingGroup2[group2Size];
	for(i=0;i<group1Size;++i){
		startingGroup1[i] = group1[i];
	}
	for(i=0;i<group2Size;++i){
		startingGroup2[i] = group2[i];
	}
	divideAndOptimize(startingGroup1, group1, group1Size, opArray, numArray, maxNumOps, opLevel*2);
	return divideAndOptimize(startingGroup2, group2, group2Size, opArray, numArray, maxNumOps, ((opLevel*2) + 1));	
}


int main(int argc, char** argv){
	int i,j; 
//	char input[] = {'\x16','\x15','\x14','\x13','\x12','\x11','\x10','\x0f'};
//	unsigned char input[] = {'\x09','\x11','\x08','\x03','\x02','\x05','\x16','\x07','\xff','\x55','\x29','\xd6','\x8f'};
//	unsigned char input[] = {'\x24','\x32','\xf7','\xc3','\x10','\x89','\xfd','\x78','\x98','\x36','\x65','\xdc','\xa4','\xb9','\xb1','\x9d'};
	unsigned char input[127];
	int inputSize = sizeof(input)/sizeof(char);
	srand(time(NULL));
	for(i=0;i<inputSize;++i){
		input[i] = rand();
		for(j=0;j<i;++j){
			if(input[i] == input[j])--i;
		}
	}
	int maxNumOps = 3;

	int success = -1;								// NEED TO IMPLEMENT divideAndOptimize in main
	size_t numBranches = 2 * highestBit(inputSize);						//number of operation sequences that divide
	char **opArray = malloc(numBranches*sizeof(char*));							//out the inputs, similar to number of 
	unsigned int ** numArray = malloc(numBranches*sizeof(unsigned int *));							//edges on a tree structure
	printf("%d",numBranches);
	return 0;
	for(i=0;i < (numBranches); ++i){
		opArray[i] = calloc(maxNumOps, sizeof(char));
		numArray[i] = calloc(maxNumOps, sizeof(int));
	}

	unsigned char startingInput[inputSize];
	for(i=0;i<inputSize;++i){
		startingInput[i] = input[i];
	}
	success = divideAndOptimize(startingInput, input, inputSize, opArray, numArray, maxNumOps,1);

	int numOps[numBranches];
	memset(numOps, 0, sizeof(numOps));
	for(j=0;j<numBranches;++j){
		for(i = 0; i<maxNumOps; ++i){
			if(numArray[j][i] != 0){
				++numOps[j];
			}
		}
	}

	if(success == 0){
		printf("Sequence Found: \n");
		printf("%s %d\n","Number of branches:", numBranches);
		for(i=1; i <numBranches; ++i){
			printf("%s %d\n\t","Branch: ", i);

			for(j=maxNumOps-numOps[i]; j<maxNumOps; ++j){
				printf("%c",opArray[i][j]); 
				printf("%d ",numArray[i][j]); 

			}
			printf("\n");

		}
//		printf("%s%d\n", "Number of Operations: ", numOps);
//		for(i=0;i < numOps[j]; ++i){
//			printf("%d%c ",i,':');
//			int opit = maxNumOps - numOps + i;
//			if(opsSeq[opit] == '&') numSeq[opit] = ~numSeq[opit];
//			printf("%c %d",opsSeq[opit], numSeq[opit]);
//			printf("\n");
//		}
	} else printf("no sequence found");
	for(i=0;i < (numBranches); ++i){
		
		free(opArray[i]);
		free(numArray[i]);
	}
	free(opArray);
	free(numArray);
	return success;
}
