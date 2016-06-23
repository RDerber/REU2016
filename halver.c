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

int compare (unsigned char* input, int inputSize, unsigned char * startingInput){
	int i,j;

/*
* Iterate through all of the input values. 
* Store an array of each output only if it is between 0 and 19. 
* Then if a second number maps to the same output as a number, check to see if they should be mapped together based on the user 	 * * specifications. 
*
*/
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
		return 0;
	}else return -1;
}

int superOptimizer (unsigned char * startingInput, unsigned char * input, int inputSize, int totOps, int numOps, char* opsSeq, int* numSeq){
	int i,j,k;
	char newinput[inputSize];
	char operations[] = {'+','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int opsMax[] = {256, 256, 256, 8, 8};
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
		if(!compare(newinput, inputSize,  startingInput)) return 0;
		else if(numOps > 1 &&
			!superOptimizer(startingInput, newinput, inputSize, totOps, numOps - 1, opsSeq, numSeq))
				return 0;
		}
	}
	return -1;
}

int main(int argc, char** argv){
	int i; 
//	char input[] = {'\x00','\x01','\x02','\x03'};
//	unsigned char input[] = {'\x24','\x32','\xf7','\xc3','\x10','\x89','\xfd','\x78','\x98','\x36','\x65','\xdc','\xa4','\xb9','\xb1','\x9d'};
	char input[] = {'\x09','\x11','\x08','\x03','\x02','\x05','\x16','\x07'};
	//char input[1000];
	int inputSize = sizeof(input)/sizeof(char);
	srand(time(NULL));
//	for(i=0;i<inputSize;++i){
//		input[i] = rand();
//	}
	int maxNumOps = 6;
	char opsSeq [maxNumOps];
	int numSeq [maxNumOps];
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}

	int success = -1;

	success = superOptimizer(input, input, inputSize, maxNumOps, maxNumOps, opsSeq, numSeq);

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
