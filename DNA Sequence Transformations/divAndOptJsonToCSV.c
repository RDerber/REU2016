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
		if(arr[i] >= high){
			high = arr[i];
			highPos = i;
		}
	}
	return highPos;
}

int timingJsonToCSV(char * input, long inputSize, FILE * ofp, int maxNumInputs, int numInputSets, int dAORuns,int evalRuns, int k){
	int i,j,h,g;
	long end = 0;
	int *inArr = malloc(maxNumInputs * sizeof(int));
	int numRuns = dAORuns + evalRuns;
	int numTimeMatches = maxNumInputs *numInputSets* numRuns;
	int status;
	regex_t re;
	regmatch_t inputMatch;
	
	char *inputPattern = "key [0-9]+"; 
	/*Number of Inputs*/
	
	if(regcomp(&re, inputPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}
	
	for(i=0; i<maxNumInputs; ++i){
		printf("%s %d\n","i:",i);
		fflush(stdout);
		status = regexec(&re, input+end, 1, &inputMatch, 0); // Find Num Inputs
		if(status != 0){
			printf("%s %s","The following expression was not found:", inputPattern);
			return -1;
		}
		long loc = inputMatch.rm_so+4;
		char c;
		int a=0;
		char numBuf[20];
//		printf("%s %d\n","loc:",loc);
		while((c = input[(loc++)+end]) != '"'){
//			printf("%c",c);
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00';
		inArr[i] = 2*atoi(numBuf);
		end += inputMatch.rm_eo;
		
		//printf("%s %d\n","i:",i);
		//printf("%s %s\n","numBuff:", numBuf);
//		printf("%s %d\n","inArr[i]:",atoi(numBuf));
//		printf("%s %d\n","begin:",inputMatch.rm_so);
//		printf("%s %d\n","end:",end);
	}
	printf("numInputs");
	fflush(stdout);
	regfree(&re);
	
	/*Time Data */
	
	regmatch_t timingMatch;
	double *timeArr = malloc(numTimeMatches*sizeof(double));
//	char *timePattern = "Run [0-9]+?\"";
	j=0;
	while(i<inputSize){
		if(input[i++] == 'R' && input[i++] == 'u' && input[i++] == 'n' && input[i++] == ' '){
			while(input[i++] != '"');
			i+=2;
			char numBuf[20];
			int a = 0;
			char c;
			while((c = input[i++]) != ',' && c!='\n' && c!='\r'){
				numBuf[a++] = c;
			}
			numBuf[a] = '\x00';
			timeArr[j++] = atof(numBuf); 
		}
	}	
	
/* Regex Timing Match
	if(regcomp(&re, timePattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}	
	
	end=0; //reset end
	for(i=0; i<numTimeMatches; ++i){
		printf("%s %d\n","i:",i);
		fflush(stdout);
		status = regexec(&re, input+end, 1, &timingMatch, 0); 
	
		if(status != 0){
			printf("%s %d", "i:", i);
			printf("%s %s","The following expression was not found:", timePattern);
			return -1;
		}
	
		long loc = timingMatch.rm_eo+2;
		char numBuf [20];
		char c;
		int a=0;
		while((c=input[(loc++)+end]) != ',' && c!='\n' && c!='\r'){
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00';
		timeArr[i] = atof(numBuf); 
		end += timingMatch.rm_eo;
	}
	printf("numTimeMatches");
	fflush(stdout);
	regfree(&re);
*/
	
	double *inputAvgRunTime = malloc (maxNumInputs * sizeof(double)); 
	double *inputAvgEvalTime = malloc(maxNumInputs*sizeof(double));

	for(i=0;i<maxNumInputs;++i){
		inputAvgEvalTime[i] = 0;
	}
	
	double avgTime = 0;
	double lowTime[k];
	for(h=0;h<maxNumInputs;h++){
		for(i=0;i<numInputSets; ++i){
			for(g=0; g<2; ++g){
				double hiloTime = DBL_MAX;
				int numStored = 0;
				int hiloPos = 0;
				double time;
				int runsVar = 0;
				if(g==0) runsVar = dAORuns;
				else runsVar = evalRuns;
				for(j=0; j<runsVar; ++j){
					//printf("%s %f\n", "hiloTime:", hiloTime);
					if((time = timeArr[h*numInputSets*numRuns+ i*numRuns+g*dAORuns+j]) < hiloTime){
						//printf("%s %f %s %d\n","time:",time, "j:",j);
						if(numStored < k){
							lowTime[numStored++] = time;
						}else {
							if((hiloPos = findHigh(lowTime,k)) < 0){
								fprintf(stderr, "%s","This really shouldn't happen");
								return -1;
							}
							lowTime[hiloPos] = time;
							hiloTime = lowTime[findHigh(lowTime,k)];
						}
						
					}
				}
				double lowSum = 0;
				for(j=0; j<k; ++j){
					lowSum += lowTime[j];
				}
				double avgLowTime = lowSum/(double)k;
				if(g==0){ //runTimes Avg - running sum
					inputAvgRunTime[h] += avgLowTime;
				}else{ // EvalTimes Avg - running sum		
					inputAvgEvalTime[h] += avgLowTime;
				}
			}
		}
	}
	
	//Find averages across input sets for each number of inputs
	
	for(h=0;h<maxNumInputs;++h){
		inputAvgRunTime[h] /= (double)numInputSets;
		inputAvgEvalTime[h] /= (double)numInputSets;
	}
	
//Print out CSV Files
	
	//Print out column headers 
	fprintf(ofp,"%c%s,%s,%s\n",'#', "Size of Key","TreeGen Time", "FileEvalTime");
	
	for(i=0;i<maxNumInputs; ++i){
		fprintf(ofp,"%d,%f,%f\n", inArr[i],inputAvgRunTime[i],inputAvgEvalTime[i]);

	}
	
	free(inArr);
	free(timeArr);
	free(inputAvgRunTime);
	free(inputAvgEvalTime);
	
	return 0;
	
}
	
	
	
int main (int argc, char * argv[]){ //[input json file] [output csv] [maxNumInputs] [numInputSets] [number of divAndOpt runs][number of evaluation runs] [lowest k values to be averaged] 

	if(argc == 8){

		FILE * ofp = fopen(argv[2], "w");
		int maxNumInputs = atoi(argv[3]);
		int numInputSets = atoi(argv[4]);
		int dAORuns = atoi(argv[5]);
		int evalRuns = atoi(argv[6]);
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
		
		timingJsonToCSV(input, inputSize, ofp, maxNumInputs,numInputSets, dAORuns, evalRuns, k);
		free(input);
		fclose(ofp);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
