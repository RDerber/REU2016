/*
* groupOptimizer.c
*
* This is another auxilary version of superOptimizer, very similar to duperOptimizer.c, which maps groups of input characters to output 
* characters.
*
* This version requires that the user hard-codes the groups into the main method and then adds them to the input array as well.
* Timing structures are also not present in this function.
*
* A predecessor to divideAndOptimize, this function was not utilitized very much. 
*
*/

#include <stdio.h>
#include <stdlib.h>

//note: function pointer method of parsing operations that is used in superoptimizer has not been implemented here. Instead, less efficient case/switch statements are used.
enum operationID {add = 0, sub = 1, and = 2, xor = 3, lsl = 4, lsr = 5};

int findMin(char* input[], int numGroups, size_t groupSizes[]){
	int min = input[0][0];
	int i;
	int j;
	for(i = 0; i<numGroups; ++i){
		for(j=0;j<groupSizes[i];++j)
			if((input[i])[j] < min) min = (input[i])[j];
	}
	return min;
}

size_t findMax(size_t * arr, size_t arrSize){
	int i;
	size_t max = arr[0];
	for(i = 1; i < arrSize; ++i){
		if(arr[i] > max) max = arr[i];
	}
	return max;
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

int compare (char* input[], int numGroups, size_t groupSizes[], char * startingInput[]){
	int i,j,k;
	int matches = 0;
	for(i=0; i < numGroups; ++i){
		for(j=0; j < numGroups; ++j){
			int equiv = 0;
			for(k = 0; k < groupSizes[j]; ++k){
				if(i == input[j][k]) ++equiv;
				else break;
			}
			if(equiv == groupSizes[j]){
				++matches;
				break;
			}
		}
		if(matches != i+1) return -1;
	}
	if(matches == numGroups){
		printf("Transformations:\nInput:\tOutput:\n");
		for(i=0;i<numGroups;++i){
			for(j=0; j<groupSizes[i]; ++j){
				printf("%d%s%d\n",startingInput[i][j],"   ->   ", input[i][j]);
			}
		}
		return 0;
	}else return -1;
}

int superOptimizer (char * startingInput[], char * input[], int numGroups, size_t groupSizes[],
			int totOps, int numOps, char* opsSeq, int* numSeq){
	int i,j,k,m;
	size_t maxGSize = findMax(groupSizes, numGroups);
	char* newinput[numGroups];
	for(i = 0; i < numGroups;++i){
		newinput[i] = malloc(groupSizes[i]*sizeof(char));
	}
	char operations[] = {'+','-','&','^','<','>'};
	int opsSize = sizeof(operations)/sizeof(char);
	int minInput = findMin(input,numGroups, groupSizes);
	//printf("%s%d\n", "minInput: ", minInput);
	int opsMax[] = {256, 256, 256, 256, 8, 8};
	for(k = 0; k < opsSize; ++k){
		int opIt = totOps - numOps;
		int constMax = opsMax[k];
//	these are possible optimizations, but in the current implementation they slow things down
//		if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
//			 (k==0 && opsSeq[opIt-1] == operations[1]) ||
//				(k==1 && opsSeq[opIt-1] == operations[0]) ||
//					(k==4 && opsSeq[opIt-1] == operations[5]) ||
//						(k==5 && opsSeq[opIt -1] == operations[4])))
//							continue;
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < numGroups; ++j){
				for(m = 0; m < groupSizes[j]; ++m){	
					newinput[j][m] = operator(input[j][m],k,i);
					opsSeq[opIt] = operations[k];
					numSeq[opIt] = i;
				}
			}
		if(!compare(newinput, numGroups, groupSizes, startingInput)) return 0;
		else if(numOps > 1 &&
			!superOptimizer(startingInput, newinput, numGroups, groupSizes, totOps, numOps - 1, opsSeq, numSeq)){
				for(i = 0; i < numGroups;++i){
					free(newinput[i]);
				}
				return 0;
			}
		}
	}
	for(i = 0; i < numGroups;++i){
		free(newinput[i]);
	}
	return -1;
}

int main(int argc, char** argv){// no arguments at the moment, groups are hard coded. simply prints out sequence found

	char group1[] = {'C','A','\x00'};
	char group2[] = {'G','T','\x00'};
	
	char *input[] = {group1,group2};
	int numGroups = sizeof(input)/sizeof(input[0]);
	size_t groupSizes[numGroups];
	int i,j; 

	for(i=0;i<numGroups;++i){
		groupSizes[i] = 0;
		j=0;
		while(input[i][j++] != '\x00'){
			++groupSizes[i];
		}
	}
	
	int maxNumOps = 6;
	char opsSeq [maxNumOps];
	int numSeq [maxNumOps] ;
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}

	int success = -1;

	success = superOptimizer(input, input, numGroups ,groupSizes, maxNumOps, maxNumOps, opsSeq, numSeq);

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
