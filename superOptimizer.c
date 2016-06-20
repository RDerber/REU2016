#include <stdio.h>
#include <stdlib.h>



int findmin(char* input, int inputSize){
	int min = input[0];
	int i;
	for(i = 1; i<inputSize; ++i){
		if(input[i] < min) min = input[i];
	}
	return min;
}

char operator(char input, int opid, int val){
	if(opid == 0){	//add
		input = input + val;

	}else if(opid == 1){
		input = input - val;

	}else if(opid == 2){
		input = input & val;

	}else if(opid == 3){
		input = input ^ val;

	}else if(opid == 4){
		input = input << val;

	}else if(opid == 5){
		input = input >> val;
	}

	return input;
}

int compare (char* input, int inputSize){
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
		for(i=0;i<inputSize;++i)
			printf("%s%d","input: ", input[i]);
		return 0;
	}
}

int superOptimizer (char * input, int inputSize, int totOps, int numOps, char* opsSeq, int* numSeq){
	int i,j,k;
	char newinput[inputSize];
	char operations[] = {'+','-','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int minInput = findmin(input,inputSize);
	//printf("%s%d\n", "minInput: ", minInput);
	int opsMax[] = {128, minInput+1, 128, 128, 16, 16};
	for(k = 0; k < opsSize; ++k){
		int constMax = opsMax[k];
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < inputSize; ++j){				
				newinput[j] = operator(input[j],k,i);
				opsSeq[totOps -  numOps] = operations[k];
				numSeq[totOps - numOps] = i;
			}
		if(!compare(newinput, inputSize)) return 0;
		else if(numOps > 1 &&
			!superOptimizer(newinput, inputSize, totOps, numOps - 1, opsSeq, numSeq))
				return 0;
		}
	}
	return -1;
}

int main(int argc, char** argv){
	char input[] = {'\x04','\x21','\x63','\x95', '\xff'};
	int inputSize = sizeof(input)/sizeof(char);
	char opsSeq [] = {' ', ' ',' '};
	int numSeq [] = {0, 0, 0};
	int numOps = sizeof(opsSeq)/sizeof(opsSeq[0]); 
	int success = -1;

	success = superOptimizer(input, inputSize, numOps, numOps, opsSeq, numSeq);

	if(success == 0){
		printf("Sequence Found: ");
		int i, j;
		printf("%s%d\n", "numOps: ", numOps);
		for(i=0;i < numOps; ++i){
			printf("%d%c ",i,':');
			printf("%c %d",opsSeq[i], numSeq[i]);
			printf("\n");
		}
	} else printf("no sequence found");
	return success;
}
