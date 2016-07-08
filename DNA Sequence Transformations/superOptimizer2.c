#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "writeJson.h"

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
//		if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
//			 (k==0 && opsSeq[opIt-1] == operations[1]) ||
//				(k==1 && opsSeq[opIt-1] == operations[0]) ||
//					(k==4 && opsSeq[opIt-1] == operations[5]) ||
//						(k==5 && opsSeq[opIt -1] == operations[4])))
//							continue;
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

// MAKE EVALUATE FUNCTION

int main(int argc, char** argv){	//[inputSize](for random inputs) or specify inputs and files to transform [input 1]...[input n][evaluateFile][outputFile]
//	char input[] = {'\x00','\x01','\x02','\x03','\x04'};
//	char input[] = {'A','C','G','T','N'};
	int i,j,k;
	int maxInputSize = 200;	//input size exceeding 200 causes our naive unique number generator to be extremely slow, exceeding 256
				//breaks the number generator
				
	int numInputSets = 10; 
	int runs = 10;
	int numDataForms = 6; //RunTime, EvalTime, OpArray, numArray,input, output
	
	char * input;
	char * output;
	int inputSize;
	double runTimes[runs];
	double evalTimes[runs];
	int maxNumOps = 6;
	
	if(argc != 3){
		printf("Please provide an inputSize and a fileSize");
		return -1;
	}
	if(atoi(argv[1]) >= maxInputSize){
		printf("inputSize too large for unique random number generator");
		return -1;
	}
	
	runTimes = malloc(runs *sizeof(double)); 
	evalTimes = malloc(runs *sizeof(double)); 
	inputSize = atoi(argv[1]);
	input = calloc(sizeof(char),inputSize);
	output = calloc(sizeof(char),inputSize);
	
	srand((unsigned int)time(0));
	for(i=0;i<inputSize;++i){
		input[i] = rand()%255;
		for(j=0;j<i;++j){
			if(input[i] == input[j]) --i;
		}
	}
		
	
	char opsSeq [maxNumOps];
	int numSeq [maxNumOps] ;
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}
		
	int success = -1;
	struct timeval time0,time1;
	for(i=0;i<runs;i++){
		gettimeofday(&time0,NULL);
		success = superOptimizer(input, input, inputSize, maxNumOps, maxNumOps, opsSeq, numSeq);
		gettimeofday(&time1,NULL);
		double runTime = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		runTimes[i] = runTime;
	}
	
	for(i=0;i<runs;i++){
		gettimeofday(&time0,NULL);
		success = evaluate(input, input, inputSize, maxNumOps, maxNumOps, opsSeq, numSeq);
		gettimeofday(&time1,NULL);
		double evalTime = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		evalTimes[i] = evalTime;
		}
	}
		
	for(i=0; i < inputSize; ++i){
		output[i] = evaluate(input[i],   );
	}

	
	double **timeArray[numDataForms];
	char *labelArray[numDataForms];	
	timeArray[0] = runTimes;
	timeArray[1] = evalTimes;
	
	labelArray[0] = "RunTimes";
	labelArray[1] = "EvalTimes";
	
	int numLabels= sizeof(labelArray); 
	
	if(write_super_file(timeArray, labelArray, numLabels, runs, opsSeq, numSeq, maxNumOps, input, output, inputSize) < 0)
		printf("error writing data file\n"); 
		
	
	free(input);	
	free(output);                            
	free(runTimes);
	free(evalTimes);

//	int numOps = 0;
//	for(i = 0; i<maxNumOps; ++i){
//		if(numSeq[i] != 0) ++numOps;
//	}

//	if(success == 0){
//		printf("Sequence Found: \n");
//		int i, j;
//		printf("%s%d\n", "Number of Operations: ", numOps);
//		for(i=0;i < numOps; ++i){
//			printf("%d%c ",i,':');
//			int opit = maxNumOps - numOps + i;
//			if(opsSeq[opit] == '&') numSeq[opit] = ~numSeq[opit];
//			printf("%c %d",opsSeq[opit], numSeq[opit]);
//			printf("\n");
//		}
//	} else printf("no sequence found");
//	return success;
	return 0;
}
