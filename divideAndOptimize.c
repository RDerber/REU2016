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
#include "writeJson.h"
//#include "divideAndOptimize.h"

enum operationID {add = 0, and = 1, xor = 2, lsl = 3, lsr = 4, assign = 5};
char operations[] = {'+','&','^','<','>','='};

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

char findFirstDiff(unsigned char input, unsigned char* arr, size_t arrSize){
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

char operate(unsigned char input, int opID, unsigned int val){
	input = functionPtrs[opID](input,val);
	return input;
}

int compare (unsigned char* input, int inputSize, unsigned char * startingInput){ // Checks to see if input values have been mapped
										// to 2 separate groups. Then returns the size of 											// the first group. 
	
	int i,j;
	char group1 = input[0];
	
	char firstDiffIndex = findFirstDiff(group1, input, inputSize);
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

int boolify(unsigned char a,unsigned char b, unsigned char *opsSeq,unsigned int *numSeq,int maxNumOps){
	int i;
	for(i=0;i<sizeof(char)*8;++i){
		unsigned int check = '\x01'<<i;
		if((a&check && !(b&check)) || (!(a&check) && b&check)){
			opsSeq[maxNumOps-2] = and;
			numSeq[maxNumOps-2] = ~check;
			opsSeq[maxNumOps-1] = lsr;
			numSeq[maxNumOps-1] = i;
			printf("%s %d","boolify:",(a&check)>>i);
			return a&check;
		}
	}
	printf("inputs should not be the same");
	return -1;
}

int superOptimizer (unsigned char * startingInput, unsigned char * input, int inputSize, int totOps, int numOps, char* opsSeq,unsigned int* numSeq){
	int i,j,k;
	char constInput[inputSize];
	for(i=0;i<inputSize;++i) constInput[i] = input[i];
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
				input[j] = operate(constInput[j],k,i);
				opsSeq[opIt] = k;
//				if(k == and) numSeq[opIt] = ~i;
//				else
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

int divideAndOptimize(unsigned char * startingInput, unsigned char* input, unsigned char *output, int inputSize,char** opArray,unsigned int** numArray, int maxNumOps, int opLevel){
	// Note: startingInput and input start out as the same thing
//Base Case: Separated into arrays of size 2
	int i;
	char opsSeq[maxNumOps];
	unsigned int numSeq[maxNumOps];
	for(i = 0; i < maxNumOps; ++i){
		opsSeq[i] = 0;
		numSeq[i] = 0;
	}

	if(inputSize == 1){
	for(i=0;i<maxNumOps;++i){
		opArray[opLevel][i] = opsSeq[i];
		numArray[opLevel][i] = numSeq[i];
	}
		opArray[opLevel][maxNumOps-1]  = assign;
		numArray[opLevel][maxNumOps-1] = output[0];
		return 0;
	}


	int group1Size = superOptimizer(startingInput, input, inputSize, maxNumOps-2, maxNumOps-2, opsSeq, numSeq);
	if(group1Size == -1) return -1;
	int group2Size = inputSize-group1Size;

//Separate inputs into groups based on the last superOptimizer run	
	unsigned char group1[group1Size];
	unsigned char group2[group2Size];
	unsigned char group1out[group1Size];
	unsigned char group2out[group2Size];
	int g1counter = 0;
	int g2counter = 0;
	int group1Check = input[0];
	int group2Check = input[findFirstDiff(group1Check,input,inputSize)];
	if(boolify(group1Check, group2Check, opsSeq, numSeq,maxNumOps)){	//finds how to make these 2 numbers 0s and 1s
		group1Check = group2Check;			//flips group 1 if it is not what becomes 0
	}

	for(i=0; i<inputSize; ++i){ 			// Sorting out original values based on what they mapped to
		if(input[i] == group1Check){		// **** Assuming that half of values will always map to 0. Is this true? 
							// Otherwise, the comparison value can be set to input[0] and start at i=1; 
			group1[g1counter] = startingInput[i];
			group1out[g1counter++] = output[i];
		}
		else{
			group2[g2counter] = startingInput[i];
			group2out[g2counter++] = output[i];
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
	divideAndOptimize(startingGroup1, group1, group1out, group1Size, opArray, numArray, maxNumOps, opLevel*2);
	return divideAndOptimize(startingGroup2, group2, group2out, group2Size, opArray, numArray, maxNumOps, ((opLevel*2) + 1));	
}


void treePrint(char** opArray, unsigned int ** numArray, size_t numBranches, int maxNumOps, int * numOps){
	int i,j;
	printf("%s %d\n","Number of branches:", numBranches);
	for(i=1; i <numBranches; ++i){
		printf("%s %d\n\t","Branch: ", i);
		for(j=maxNumOps-numOps[i]; j<maxNumOps; ++j){
			printf("%c",operations[opArray[i][j]]); 
			printf("%d ",numArray[i][j]); 

		}
		printf("\n");

	}

}

int codonIdentifier(FILE * ifp,unsigned char *input,unsigned char *output){
	char byt; 
	int inputCount = 0; 
	int outputCount = 0;
	while((byt=getc(ifp))!= EOF){
	
		if(byt != ' ' && byt != '\n' && byt != '\r' && byt!= ','){
			if(byt != '>'){
				byt = byt & '\x06';
				byt = byt << 3;
		
				char temp = getc(ifp);
			
				temp =  temp & '\x06';
				temp = temp <<1; 
				byt = byt | temp; 
			

				temp = getc(ifp);
				temp =  temp & '\x06';
				temp = temp >> 1; 
				byt = byt | temp; 
			
				input[inputCount++] = byt; 
			}else{
				byt = getc(ifp);
				for(outputCount; outputCount < inputCount; ++outputCount){
					output[outputCount] = byt;
				}
			}
		}
	}
	return inputCount;
} 

int evaluate(unsigned char input, char** opArray, unsigned int ** numArray, int maxNumOps){
	int i = 1,j;
	unsigned char direction;
	while(opArray[i][maxNumOps-1] != assign){
		direction = input;
		for(j=0;j < maxNumOps;++j){
			direction = operate(direction,((opArray[i])[j]),((numArray[i])[j]));
		//	printf("%s%d %s%d\n","opArray[i][j]: ",opArray[i][j], "numArray[i][j] ",numArray[i][j]);
		//	printf("%s%d %s%d\n","j: ",j, "direction: ",direction);
		}
	//	printf("%s%d %s%d\n","i: ",i, "direction: ",direction);
		fflush(stdout);
		i = i*2 + direction;
	}
	int output = numArray[i][maxNumOps-1];
	printf("%s %c %s %c\n","Input:", input, "Output:", output);
	return output;
}

int main(int argc, char** argv){ // [key][inputFile][outputFile][number of runs][number of timing values to retain]
	int i,j; 
//	char input[] = {'\x16','\x15','\x14','\x13','\x12','\x11','\x10','\x0f'};
	
//	unsigned char input[] = {'\x01','\x05','\x10','\x15','\x20','\x25','\x30','\x09'};
//	unsigned char output[] = {'\x02','\x07','\x12','\x17','\x22','\x27','\x32','\x37'};
	int inputSize = 0; 
	FILE * ifp;
	ifp = fopen(argv[1],"r");
	if(ifp != NULL){
	 // Go to the end of the file //
		if(fseek(ifp, 0L, SEEK_END)== 0){
		// Get the size of the file. //

			inputSize = ftell(ifp);
			if (inputSize == -1) {
				fputs("Error finding size of file", stderr);
			}

			// Return to start of file //
			if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
				fputs("Error returning to start of file", stderr);
			}
		}
	} else{ 
		printf("File not accessible.");	
		return -1; 
		}
		
	unsigned char * input = malloc(inputSize * sizeof(char));
	unsigned char * output = malloc(inputSize * sizeof(char));
	
	inputSize = codonIdentifier(ifp, input, output);
	fclose(ifp);
//	int inputSize = 128;
//	unsigned char *input = malloc(sizeof(char)*inputSize);
//	unsigned char *output = malloc(sizeof(char)*inputSize);
//	srand(time(NULL));

//	int inputSize = sizeof(input)/sizeof(input[0]);
//	for(i=0;i<inputSize;++i){
//		input[i] = rand();
//		output[i] = rand();
//		for(j=0;j<i;++j){
//			if(input[i] == input[j])--i;
//		}
//	}




	int maxNumOps = 5;			//must be at least 4 to store comparisions

	int success = -1;								// NEED TO IMPLEMENT divideAndOptimize in main
	size_t numBranches = 2 * highestBit(inputSize);						//number of operation sequences that divide
	char **opArray = malloc(numBranches*sizeof(char*));							//out the inputs, similar to number of 
	unsigned int ** numArray = malloc(numBranches*sizeof(unsigned int *));							//edges on a tree structure
	for(i=0;i < (numBranches); ++i){
		opArray[i] = calloc(maxNumOps, sizeof(char));
		numArray[i] = calloc(maxNumOps, sizeof(int));
	}

	unsigned char startingInput[inputSize];
	for(i=0;i<inputSize;++i){
		startingInput[i] = input[i];
	}
	success = divideAndOptimize(startingInput, input, output, inputSize, opArray, numArray, maxNumOps,1);

	free(input);
	free(output);

	int numOps[numBranches];
	memset(numOps, 0, sizeof(numOps));
	for(j=0;j<numBranches;++j){
		for(i = 0; i<maxNumOps; ++i){
			if(opArray[j][i] != ' ' && !(opArray[j][i] == 0 && numArray[j][i] == 0)){
				++numOps[j];
			}
		}
	}

	if(success == 0){
		printf("Sequence Found: \n");

		treePrint(opArray, numArray, numBranches, maxNumOps, numOps); 


//Map codons to amino acids
		
		// Create Input Memory Buffer //
	char *readIn = NULL;
	FILE *ifp = fopen(argv[2],"r");
	long readSize = 0;
	if(ifp != NULL){
	 // Go to the end of the file //
	if(fseek(ifp, 0L, SEEK_END)== 0){
		// Get the size of the file. //

		readSize = ftell(ifp);
			if (readSize == -1) {
				fputs("Error finding size of file", stderr);
			}
	
	
		readIn = malloc (sizeof(char) * (readSize+1));

			// Return to start of file //
			if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
				fputs("Error returning to start of file", stderr);
			}
		}
	}
	else{
		fputs("Error reading input file", stderr);
	}

	// Tranlate Read File to codon 2-Bit format
		char empty[1] = {};
		readSize = codonIdentifier(ifp, readIn, empty);

		// Create Output Buffer;
	char * writeOut = malloc(sizeof(char)* (readSize+1));
	int writeSize = readSize;
	
	float *times;
	int runs = 0;
	int numTimes = 0;
	if(argc == 4){
		for(i=0;i<readSize; ++i){
		writeOut[i] = evaluate(readIn[i],opArray,numArray,maxNumOps);
		}
	}
	if(argc == 5){	//if a number of runs is given but no number of minimum times, default number of min times is 3
		runs = atoi(argv[4]);
		numTimes = 3;
		times = calloc(runs, sizeof(float)); 
		struct timeval time0, time1; 
		int i;
		for(i=0;i<runs;i++){ // Record time of each run
			gettimeofday(&time0,NULL);
			for(i=0;i<readSize; ++i){
				writeOut[i] = evaluate(readIn[i],opArray,numArray,maxNumOps);
			}
			gettimeofday(&time1,NULL);
			times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		}

	}
	if(argc == 6){ //if both number of runs and the number of minimum times is given
		runs = atoi(argv[4]);
        numTimes = atoi(argv[5]);
        times = calloc(runs, sizeof(float));
        struct timeval time0, time1; 
        int i;
        for(i=0;i<runs;i++){ // Record time of each run
                gettimeofday(&time0,NULL);
                for(i=0;i<readSize; ++i){
					writeOut[i] = evaluate(readIn[i],opArray,numArray,maxNumOps);
				}
                gettimeofday(&time1,NULL);
                times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
                }

	}

	// JSON timing.txt file output if [runs] and [num min times] arguments are included // 
	if(argc > 4){
		if(write_time_file(times, runs, numTimes,readSize) < 0)
			printf("error writing time file\n");
		free(times);
	}
	// Writing output buffer to specified output file//
	FILE *ofp = fopen(argv[3],"w");
	if(ofp == NULL){
		printf("Error creating output file\n");
		return -1;
        }else{
		 fwrite(writeOut, 1, writeSize, ofp);
		fclose(ofp);
		}
	free(readIn);
	free(writeOut);

//		printf("%s %d\n","Number of branches:", numBranches);
//		for(i=1; i <numBranches; ++i){
//			printf("%s %d\n\t","Branch: ", i);
//
//			for(j=maxNumOps-numOps[i]; j<maxNumOps; ++j){
//				printf("%c",operations[opArray[i][j]]); 
//				printf("%d ",numArray[i][j]); 
//
//			}
//			printf("\n");
//
//		}
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
