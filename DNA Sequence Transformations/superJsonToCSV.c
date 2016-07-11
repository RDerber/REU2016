/*
*jsonToCSV.c

*FIX REGEX PATTERN 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <regex.h>

int findHigh(double * arr, size_t arrSize){
	int i;
	int high = DBL_MIN;
	int highPos = -1;
	for(i=0;i<arrSize;++i){
		if(arr[i] > high){
			high = arr[i];
			highPos = i;
		}
	}
	return highPos;
}

int jsonToCSV(char * input, FILE * ofp1, FILE* ofp2, int maxNumInputs, int numInputSets, int m, int k, long inputSize){
	int i,j,h,g;
	int inArr[maxNumInputs];
	int numTimeMatches = maxNumInputs * numInputSets * m * 2;
	int status;
	regex_t re;
	regmatch_t inputMatches[maxNumInputs];
	
	char inputPattern[] = "([0-9][0-9]?[0-9]? input\(s\))"; 
	/*Number of Inputs*/
	if(regcomp(&re, inputPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}
	
	status = regexec(&re, input, maxNumInputs, inputMatches, 0); // Find Num Inputs
	
	if(status != 0){
		printf("%s %s","The following expression was not found:", inputPattern);
		return -1;
	}
	

	for(i=0; i<maxNumInputs; ++i){
		int loc = inputMatches[i].rm_so;
		char c;
		int a=0;
		char numBuf[20];
		while((c = input[loc++]) != ' '){
			numBuf[a++] = c;
		}
		inArr[i] = atoi(numBuf);
	}
	
	
	
	
	/*Time Data (RunTime and Eval Time Alternating)*/
	
	regmatch_t timingMatches[numTimeMatches];
	double timeArr[numTimeMatches];
	char timePattern[] = "(\"Run [0-9][0-9]?[0-9]?\": )";
	
	if(regcomp(&re, timePattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}	
	
	status = regexec(&re, input, numTimeMatches, timingMatches, 0); 
	
	if(status != 0){
		printf("%s %s","The following expression was not found:", timePattern);
		return -1;
	}
	
	for(i=0; i<numTimeMatches; ++i){
		int begin = timingMatches[i].rm_eo;
		char numBuf [20];
		j=0;
		while(input[begin] != ','){
			numBuf[j++] = input[begin++];
		}
		timeArr[i] = atoi(numBuf); 
	}
	
	/*NumOps*/
	int metaNumOps = maxNumInputs *numInputSets;
	regmatch_t numOpMatches[metaNumOps];
	char numOpPattern[] = "(\"numOps\": )";
	double numOpArr[metaNumOps];
	
	if(regcomp(&re, numOpPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}	
	
	status = regexec(&re, input, metaNumOps, numOpMatches, 0); 
	
	if(status != 0){
		printf("%s %s","The following expression was not found:", numOpPattern);
		return -1;
	}
	
	for(i=0; i<metaNumOps; ++i){
		int begin = numOpMatches[i].rm_eo;
		char numBuf [10];
		j=0;
		while(input[begin] != ','){
			numBuf[j++] = input[begin++];
		}
		numOpArr[i] = atoi(numBuf); 
	}
	
	
	
	double numOpAvgRunTime [metaNumOps];
	double numOpAvgEvalTime [metaNumOps];
	double inputAvgRunTime [maxNumInputs];
	double inputAvgEvalTime [maxNumInputs];
	
	int numOpCount[metaNumOps];
	
	double avgTime = 0;
	double lowTime[k];
	for(h=0;h<maxNumInputs;h++){
		for(i=0;i<numInputSets; ++i){
			for(g=0; g<2; ++g){
				double hiloTime = DBL_MAX;
				int numStored = 0;
				int hiloPos = 0;
				double time;
				for(j=0; j<m; ++j){
					if((time = timeArr[h*numInputSets*m*2 + i*m + j]) < hiloTime){
						if(numStored < k){
							lowTime[numStored++] = time;
						}else {
							if((hiloPos = findHigh(lowTime,k)) < 0){
								fprintf(stderr, "%s","This really shouldn't happen");
								return -1;
							}
						}
						lowTime[hiloPos] = time;
					}
				}
				int lowSum = 0;
				for(j=0; j<k; ++j){
					lowSum += lowTime[k];
				}
				int avgLowTime= lowSum/k; 
				
				if(g==0){ //runTimes Avg - running sum
					int numOps = numOpArr[h*numInputSets+i];
					numOpAvgRunTime[numOps] += avgLowTime;
					numOpCount[numOps]++;
				
					inputAvgRunTime[h] += avgLowTime;
				
				}else{ // EvalTimes Avg - running sum
					int numOps = numOpArr[h*numInputSets+i];
					numOpAvgEvalTime[numOps] += avgLowTime;			
					inputAvgEvalTime[h] += avgLowTime;
				}
			}
		}
		
	}
	
	//Find averages
	
	for(h=0;h<maxNumInputs;++h){
		inputAvgRunTime[h] /= numInputSets;
		inputAvgEvalTime[h] /= numInputSets;
	}
	
	int maxNumOps = 0;
	i=0;
	while(numOpCount[i++]!=0)
		maxNumOps++;
		
	for(i=0;i<maxNumOps;++i){
		if(numOpCount[i] != 0){
			numOpAvgRunTime[i] /= numOpCount[i];
			numOpAvgEvalTime[i] /= numOpCount[i];
		}
		
	}
			
	//Print out CSV Files
	
	
	for(i=0;i<maxNumInputs; ++i){ 
		fprintf(ofp1,"%d, %f, %f, %f\n", inArr[i], numOpArr[i],inputAvgRunTime[i],inputAvgEvalTime[i]);
	}
	
	fprintf(opf2,"%s","Testing");
	
	return 0;
	
}
	
	
	
int main (int argc, char * argv[]){ //[input json file] [output csv filename1] [output csv filename2] [maxNumInputs] [numInputSets] [m number of runs] [lowest k values to be averaged] 

	if(argc == 8){

		FILE * ofp1 = fopen(argv[2], "w");
		FILE * ofp2 = fopen(argv[3], "w");
		int maxNumInputs = atoi(argv[4]);
		int numInputSets = atoi(argv[5]);
		int m = atoi(argv[6]);
		int k = atoi(argv[7]);
		
		// Create Input Memory Buffer //
		char *input = NULL;
		FILE *ifp = fopen(argv[1],"r");
		long inputSize = 0;
		if(ifp != NULL){
			 // Go to the end of the file //
			if(fseek(ifp, 0L, SEEK_END)== 0){
				// Get the size of the file. //
	
				inputSize = ftell(ifp);
				if (inputSize == -1) {
					fputs("Error finding size of file", stderr);
				 }
			
				//Allocate our buffer of that size +1 for null termination. //
				input = malloc (sizeof(char) * (inputSize+1));
				
		
				// Return to start of file //
				if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
					fputs("Error returning to start of file", stderr);
				}
		
				//Read the entire file into memory//
				size_t newLen = fread(input, sizeof(char), inputSize, ifp);
				if(newLen == 0){
					fputs("Error reading file", stderr);
				} else {
					input[newLen++] = '\0'; // Null termination character at the end of the input buffer 
					}
			}			
			fclose(ifp);
		}else{
			printf("%s\n", "the input file given does not exist");
			return -1;
		}
		
		
		jsonToCSV(input,ofp1, ofp2,maxNumInputs,numInputSets,m,k,inputSize);

		free(input);
		fclose(ofp1);
		fclose(ofp2);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
