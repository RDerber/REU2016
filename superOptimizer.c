#include <stdio.h>
#include <stdlib.h>

enum operationID {add = 0, sub = 1, and = 2, xor = 3, lsl = 4, lsr = 5};

int findmin(char* input, int inputSize){
	int min = input[0];
	int i;
	for(i = 1; i<inputSize; ++i){
		if(input[i] < min) min = input[i];
	}
	return min;
}

char operator(char input, enum operationID opID, int val){

	switch(opID){
		case add:
			input = input + val;
			break;

		case sub:
			input = input - val;
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

int compare (char* input, int inputSize, char * startingInput){
	int i;
	int j;
	int matches = 0;
	for(i=0; i < inputSize; ++i){
		for(j=0; j < inputSize; ++j){
			if(i == input[j]){
				++matches;
				continue;
			}
		}
		if(matches != i+1) return -1;
	}
	if(matches == inputSize){
		printf("Transformations:\nInput:\tOutput:\n");
		for(i=0;i<inputSize;++i)
			printf("%d%s%d\n",startingInput[i],"   ->   ", input[i]);
		return 0;
	}
}

int superOptimizer (char * startingInput, char * input, int inputSize, int totOps, int numOps, char* opsSeq, int* numSeq){
	int i,j,k;
	char newinput[inputSize];
	char operations[] = {'+','-','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int minInput = findmin(input,inputSize);
	//printf("%s%d\n", "minInput: ", minInput);
	int opsMax[] = {128, minInput+1, 128, 128, 8, 8};
	for(k = 0; k < opsSize; ++k){
		int opIt = totOps - numOps;
		int constMax = opsMax[k];
		if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
			 (k==0 && opsSeq[opIt-1] == operations[1]) ||
				(k==1 && opsSeq[opIt-1] == operations[0]) ||
					(k==4 && opsSeq[opIt-1] == operations[5]) ||
						(k==5 && opsSeq[opIt -1] == operations[4])))
							continue;
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < inputSize; ++j){				
				newinput[j] = operator(input[j],k,i);
				opsSeq[opIt] = operations[k];
				numSeq[opIt] = i;
			}
		if(!compare(newinput, inputSize, startingInput)) return 0;
		else if(numOps > 1 &&
			!superOptimizer(startingInput, newinput, inputSize, totOps, numOps - 1, opsSeq, numSeq))
				return 0;
		}
	}
	return -1;
}

int main(int argc, char** argv){
//	char input[] = {'\x00','\x01','\x02','\x03','\x04'};
	char input[] = {'A','C','G','T'};
	int inputSize = sizeof(input)/sizeof(char);
	int maxNumOps = 6;
	char opsSeq [maxNumOps];
	int numSeq [maxNumOps] ;
	int i; 
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}

	int success = -1;

	success = superOptimizer(input, input, inputSize, maxNumOps, maxNumOps, opsSeq, numSeq);

	int numOps = 0;
	for(i = 0; i<maxNumOps; ++i){
		if(numSeq[i] != 0) ++numOps;
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
