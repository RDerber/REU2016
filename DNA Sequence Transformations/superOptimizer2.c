#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "jsonData.h"

enum operationID {add = 0, and = 1, xor = 2, or = 3, lsl = 4, lsr = 5};
char operations[] = {'+','&','^','|','<','>'};

/*These functions allow us to use an array of function pointers pointing to these functions in order to identify and perform the operation in constant time and without the use of a switch case statement. */
unsigned char addFunc(unsigned char a, unsigned int b){
	return a+b;
}

unsigned char andFunc(unsigned char a, unsigned int b){
	return a&b;
}

unsigned char xorFunc(unsigned char a, unsigned int b){
	return a^b;
}
unsigned char orFunc(unsigned char a, unsigned int b){
	return a|b;
}
unsigned char shiftLeftFunc(unsigned char a, unsigned int b){
	unsigned char ans = a << b;
	return ans;
}

unsigned char shiftRightFunc(unsigned char a, unsigned int b){
	unsigned char ans = a >> b;	
	return ans; 
}

unsigned char (*functionPtrs[])(unsigned char,unsigned int) = {&addFunc,&andFunc, &xorFunc, &orFunc, &shiftLeftFunc, &shiftRightFunc};

//uses opID to call the funtion in functionPtrs on the input values.
unsigned char operator(unsigned char input, enum operationID opID, int val){
	input = functionPtrs[opID](input,val);
	return input;
}

/*checks to see if the inputs are minimal perfect hashed*/
int compare (unsigned char* input, int numDistInputs, unsigned char * startingInput){
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

//an exhaustive search of all sequences of operations, returning when the specifications set by compare are met.
int superOptimizer (unsigned char * startingInput,unsigned char * input, int numDistInputs, int totOps, int numOps, char* opsSeq, int* numSeq){
	unsigned int i,j,k;
	unsigned char newinput[numDistInputs];
	int opsSize = sizeof(operations)/sizeof(char);
	int opsMax[] = {256, 256, 256, 256, 8, 8};
	
	for(k = 0; k < opsSize; ++k){
		int opIt = totOps - numOps;
		int constMax = opsMax[k];
		
/*Below are theorietically optimizations, preventing redundant operatios from being tested (ex: lsl then lsr is redundant with & and shift) but with the current implementation they break the program.*/

//		if(opIt > 0 && (opsSeq[opIt -1] == operations[k] ||
//			 (k==0 && opsSeq[opIt-1] == operations[1]) ||
//				(k==1 && opsSeq[opIt-1] == operations[0]) ||
//					(k==4 && opsSeq[opIt-1] == operations[5]) ||
//						(k==5 && opsSeq[opIt -1] == operations[4])))
//							continue;
		for(i = 0; i < constMax; ++i){
			for(j = 0; j < numDistInputs; ++j){
				opsSeq[opIt] = k;
				if(k == and){
					newinput[j] = operator(input[j],k,255-i);
					numSeq[opIt] = 255-i;
				}else{
					newinput[j] = operator(input[j],k,i);
					numSeq[opIt] = i;
				}
			}
		if(!compare(newinput, numDistInputs, startingInput)) return 0;
		else if(numOps > 1 &&
			!superOptimizer(startingInput, newinput, numDistInputs, totOps, numOps - 1, opsSeq, numSeq))
				return 0;
		}
	}
	return -1;
}


char evaluate(int input, char * opsSeq, int * numSeq, int maxNumOps, int numOps){
	int i,j;
	unsigned char output = input; 
	for(i = maxNumOps-numOps;i<maxNumOps;++i)
		output = operator(output, opsSeq[i], numSeq[i]);
	return output;
}

int main(int argc, char** argv){	//[number of Distinct Inputs][file size][random seed] [num of superOptruns] [num of EvalRuns]
//	char input[] = {'\x00','\x01','\x02','\x03','\x04'};
//	char input[] = {'A','C','G','T','N'};
	int i,j,k;
	int maxNumDistInputs = 200;//input size exceeding 200 causes our naive unique number generator to be extremely slow, exceeding 256
				//breaks the number generator

	int superOptRuns = atoi(argv[4]);
	int evalRuns = atoi(argv[5]);
	int numDataForms = 2; //runTime, evalTime
	
	unsigned char * input;
	unsigned char * output;
	int numDistInputs;
	double runTimes[superOptRuns];
	double evalTimes[evalRuns];
	int maxNumOps = 6;
	long fileSize = atoi(argv[2]);
	
	if(argc != 6){
		printf("Bad arguments");
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
			if(input[i] == input[j]){
				--i;
				break;
			}
		}
	}
	
	char opsSeq [maxNumOps];
	char opRep [maxNumOps];
	int numSeq [maxNumOps] ;
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = add;
		numSeq[i] = 0;
	}
	char * testBuf = malloc(fileSize*sizeof(char));
	srand(time(0));
	for(i=0;i<fileSize;++i){
		testBuf[i] = input[rand()%numDistInputs];
	}
	int success = -1;
	struct timeval time0,time1;
	for(i=0;i<superOptRuns;i++){
		gettimeofday(&time0,NULL);
		success = superOptimizer(input, input, numDistInputs, maxNumOps, maxNumOps, opsSeq, numSeq);
		gettimeofday(&time1,NULL);
		double runTime = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		runTimes[i] = runTime;
	}
	int numOps = 0;
	for(i=0;i<maxNumOps;++i){
		if(numSeq[i] != 0){
			opRep[i] = operations[opsSeq[i]];
			++numOps;
		}else{
			opRep[i] = ' ';
		}
	}
	
	for(i=0;i<evalRuns;i++){
		gettimeofday(&time0,NULL);
		for(j = 0; j < fileSize; ++j){
			evaluate(testBuf[j], opsSeq, numSeq, maxNumOps, numOps);
		}
		gettimeofday(&time1,NULL);
		double evalTime = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		evalTimes[i] = evalTime;
	}
	free(testBuf);	

	for(i=0; i < numDistInputs; ++i){
		output[i] = evaluate(input[i], opsSeq, numSeq,maxNumOps, numOps);
	}

	
	double *timeArray[numDataForms];
	char *labelArray[numDataForms];	
	timeArray[0] = runTimes;
	timeArray[1] = evalTimes;
	
	labelArray[0] = "RunTimes";
	labelArray[1] = "EvalTimes";
	
	int numLabels= sizeof(labelArray)/sizeof(char*); 
	
	int runsArray[numDataForms];
	runsArray[0] = superOptRuns; 
	runsArray[1] = evalRuns; 
	
	if(write_super_file(timeArray, labelArray, numLabels, runsArray, opRep, numSeq, maxNumOps, input, output, numDistInputs) < 0)
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
