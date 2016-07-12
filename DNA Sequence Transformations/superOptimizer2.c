#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "jsonData.h"

enum operationID {add = 0, sub = 1, and = 2, xor = 3, lsl = 4, lsr = 5};
char operations[] = {'+','-','&','^','<','>'};

unsigned char addFunc(unsigned char a, unsigned int b){
	return a+b;
}

unsigned char andFunc(unsigned char a, unsigned int b){
	return a&(~b);
}

unsigned char xorFunc(unsigned char a, unsigned int b){
	return a^b;
}

unsigned char shiftLeftFunc(unsigned char a, unsigned int b){
	unsigned char ans = a << b;
	return ans;
}

unsigned char shiftRightFunc(unsigned char a, unsigned int b){
	unsigned char ans = a >> b;	
	return ans; 
}

unsigned char assignFunc(unsigned char a, unsigned int b){
	return b;
}

unsigned char (*functionPtrs[])(unsigned char,unsigned int) = {&addFunc,&andFunc,&xorFunc,&shiftLeftFunc,&shiftRightFunc,&assignFunc};

int findmin(char* input, int numDistInputs){
	int min = input[0];
	int i;
	for(i = 1; i<numDistInputs; ++i){
		if(input[i] < min) min = input[i];
	}
	return min;
}

char operator(char input, enum operationID opID, int val){
	input = functionPtrs[opID](input,val);
	return input;
}

int compare (char* input, int numDistInputs, char * startingInput){
	int i;
	int j;
	int matches = 0;
	for(i=0; i < numDistInputs; ++i){
		for(j=0; j < numDistInputs; ++j){
			if(i == input[j]){
				++matches;
				continue;
			}
		}
		if(matches != i+1) return -1;
	}
	if(matches == numDistInputs){
		//printf("Transformations:\nInput:\tOutput:\n");
		//for(i=0;i<numDistInputs;++i)
		//	printf("%d%s%d\n",startingInput[i],"   ->   ", input[i]);
		return 0;
	}
}

int superOptimizer (char * startingInput, char * input, int numDistInputs, int totOps, int numOps, char* opsSeq, int* numSeq){
	int i,j,k;
	char newinput[numDistInputs];
	int opsSize = sizeof(operations)/sizeof(char);
	int minInput = findmin(input,numDistInputs);
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
			for(j = 0; j < numDistInputs; ++j){
				newinput[j] = operator(input[j],k,i);
				opsSeq[opIt] = k;
				numSeq[opIt] = i;
			}
		if(!compare(newinput, numDistInputs, startingInput)) return 0;
		else if(numOps > 1 &&
			!superOptimizer(startingInput, newinput, numDistInputs, totOps, numOps - 1, opsSeq, numSeq))
				return 0;
		}
	}
	return -1;
}


char evaluate(int input, char * opsSeq, int * numSeq, int maxNumOps){
	int i,j;
	char output;
	for(i=0;i<maxNumOps;++i)
		output = operator(input, opsSeq[i], numSeq[i]);
	return output;
}

int main(int argc, char** argv){	//[number of Distinct Inputs][file size][random seed](for random inputs and file size) or specify inputs and files to transform [input 1]...[input n][evaluateFile][outputFile]
//	char input[] = {'\x00','\x01','\x02','\x03','\x04'};
//	char input[] = {'A','C','G','T','N'};
	int i,j,k;
	int maxNumDistInputs = 200;	//input size exceeding 200 causes our naive unique number generator to be extremely slow, exceeding 256
				//breaks the number generator

	int runs = 10;
	int numDataForms = 2; //RunTime, EvalTime, OpArray, numArray,input, output
	
	char * input;
	char * output;
	int numDistInputs;
	double runTimes[runs];
	double evalTimes[runs];
	int maxNumOps = 6;
	long fileSize = atoi(argv[2]);
	
	if(argc != 4){
		printf("Please provide a numDistInputs and a fileSize");
		return -1;
	}
	if(atoi(argv[1]) >= maxNumDistInputs){
		printf("numDistInputs too large for unique random number generator");
		return -1;
	}
	 
	numDistInputs = atoi(argv[1]);
	input = calloc(sizeof(char),numDistInputs);
	output = calloc(sizeof(char),numDistInputs);
	
	srand(atoi(argv[3]));
	for(i=0;i<numDistInputs;++i){
		input[i] = rand()%255;
		for(j=0;j<i;++j){
			if(input[i] == input[j]) --i;
		}
	}
	
	char opsSeq [maxNumOps];
	char opRep [maxNumOps];
	int numSeq [maxNumOps] ;
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = ' ';
		numSeq[i] = 0;
	}
	char * testBuf = malloc(fileSize*sizeof(char));
	srand(time(0));
	for(i=0;i<fileSize;++i){
		testBuf[i] = input[rand()%numDistInputs];
	}
	int success = -1;
	struct timeval time0,time1;
	for(i=0;i<runs;i++){
		gettimeofday(&time0,NULL);
		success = superOptimizer(input, input, numDistInputs, maxNumOps, maxNumOps, opsSeq, numSeq);
		gettimeofday(&time1,NULL);
		double runTime = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		runTimes[i] = runTime;
	}
	
	for(i=0;i<maxNumOps;++i){
		if(numSeq[i] != 0){
			opRep[i] = operations[opsSeq[i]];
		}else{
			opRep[i] = ' ';
		}
	}
	
	for(i=0;i<runs;i++){
		gettimeofday(&time0,NULL);
		for(j = 0; j < fileSize; ++j){
			evaluate(testBuf[j], opsSeq, numSeq, maxNumOps);
		}
		gettimeofday(&time1,NULL);
		double evalTime = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		evalTimes[i] = evalTime;
	}
	free(testBuf);	

	for(i=0; i < numDistInputs; ++i){
			output[i] = evaluate(input[i], opsSeq, numSeq,maxNumOps);
	}

	
	double *timeArray[numDataForms];
	char *labelArray[numDataForms];	
	timeArray[0] = runTimes;
	timeArray[1] = evalTimes;
	
	labelArray[0] = "RunTimes";
	labelArray[1] = "EvalTimes";
	
	int numLabels= sizeof(labelArray)/sizeof(char*); 
	
	if(write_super_file(timeArray, labelArray, numLabels, runs, opRep, numSeq, maxNumOps, input, output, numDistInputs) < 0)
		printf("error writing data file\n"); 
		
	free(input);
	free(output);


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
