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

int superJsonToCSV(char * input, FILE * ofp1, FILE* ofp2, int maxNumInputs, int numInputSets, int m, int k, long inputSize){
	int i,j,h,g;
	int end = 0;
	int inArr[maxNumInputs];
	int numTimeMatches = maxNumInputs * numInputSets * m * 2;
	int status;
	regex_t re;
	regmatch_t inputMatch;
	
	char *inputPattern = "[0-9]+ input"; 
	/*Number of Inputs*/
	
	if(regcomp(&re, inputPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}
	
	
	for(i=0; i<maxNumInputs; ++i){
		status = regexec(&re, input+end, 1, &inputMatch, 0); // Find Num Inputs
		if(status != 0){
			printf("%s %s","The following expression was not found:", inputPattern);
			return -1;
		}
		int loc = inputMatch.rm_so;
		char c;
		int a=0;
		char numBuf[20];
//		printf("%s %d\n","loc:",loc);
		while((c = input[(loc++)+end]) != '"'){
//			printf("%c",c);
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00';
		inArr[i] = atoi(numBuf);
		end += inputMatch.rm_eo;
		
		//printf("%s %d\n","i:",i);
		//printf("%s %s\n","numBuff:", numBuf);
//		printf("%s %d\n","inArr[i]:",atoi(numBuf));
//		printf("%s %d\n","begin:",inputMatch.rm_so);
//		printf("%s %d\n","end:",end);
	}
	
	regfree(&re);
	
	/*Time Data (RunTime and Eval Time Alternating)*/
	
	regmatch_t timingMatch;
	double timeArr[numTimeMatches];
	char *timePattern = "Run [0-9]+";
	
	if(regcomp(&re, timePattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}	
	
	end=0; //reset end
	for(i=0; i<numTimeMatches; ++i){
		status = regexec(&re, input+end, 1, &timingMatch, 0); 
	
		if(status != 0){
			printf("%s %s","The following expression was not found:", timePattern);
			return -1;
		}
	
		int loc = timingMatch.rm_eo+3;
		char numBuf [20];
		char c;
		int a=0;
		while((c=input[(loc++)+end]) != ','){
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00';
		timeArr[i] = atoi(numBuf); 
		end += timingMatch.rm_eo;
//		printf("%s %d\n","i:",i);
//		printf("%s %s\n","numBuff:", numBuf);
//		printf("%s %d\n","timeArr[i]:",atoi(numBuf));
		
//		printf("%s %d\n","begin:",timingMatch.rm_so+end);
//		printf("%s %d\n","end:",end);
	}
	
	regfree(&re);
	
	/*NumOps*/
	int metaNumOps = maxNumInputs *numInputSets;
	regmatch_t numOpMatch;
	char *numOpPattern = "numOps";
	int numOpArr[metaNumOps];
	for(i=0;i<metaNumOps;++i){
		numOpArr[i] = 0;
	}
	
	if(regcomp(&re, numOpPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}	
	
	end=0;
	for(i=0; i<metaNumOps; ++i){
		status = regexec(&re, input+end, 1, &numOpMatch, 0); 
	
		if(status != 0){
			printf("%s %s","The following expression was not found:", numOpPattern);
			return -1;
		}
		int loc = numOpMatch.rm_eo + 3;
		char numBuf [10];
		char c;
		int a=0;
		while((c=input[(loc++)+end]) != ','){
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00'; 
		numOpArr[i] = atoi(numBuf); 
		end += numOpMatch.rm_eo;
//		printf("%s %d\n","i:",i);
//		printf("%s %s\n","numBuff:", numBuf);
//		printf("%s %d\n","OpArr[i]:",atoi(numBuf));
//		printf("%s %d\n","begin:",numOpMatch.rm_so+end);
//		printf("%s %d\n","end:",end);
	}
	
	regfree(&re);
	
	double numOpAvgRunTime [metaNumOps];
	double numOpAvgEvalTime [metaNumOps];
	double inputAvgRunTime [maxNumInputs];
	double inputAvgEvalTime [maxNumInputs];
	
	double numOpCount[metaNumOps];
	for(i=0;i<metaNumOps;++i){
		numOpCount[i]=0;
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
				for(j=0; j<m; ++j){
					//printf("%s %f\n", "hiloTime:", hiloTime);
					if((time = timeArr[h*numInputSets*m*2 + i*m*2 +j+g*m]) < hiloTime){
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
				double avgLowTime= lowSum/(double)k; 
				if(g==0){ //runTimes Avg - running sum
					int numOps = numOpArr[h*numInputSets+i];
					numOpAvgRunTime[numOps-1] += avgLowTime;
					numOpCount[numOps-1]++;
					//printf("%s %d %s %f\n","numOps:",numOps,"numOpAvgRunTime[numOps]:",numOpAvgRunTime[numOps]);
					inputAvgRunTime[h] += avgLowTime;
					//printf("%s %d %s %f\n","numInputs:",h,"InputAvgRunTime[numOps]:",inputAvgRunTime[h]);
					//printf("%s %d %s %f\n","g:",g,"avgLowTime",avgLowTime);
				}else{ // EvalTimes Avg - running sum
					int numOps = numOpArr[h*numInputSets+i];
					numOpAvgEvalTime[numOps-1] += avgLowTime;			
					inputAvgEvalTime[h] += avgLowTime;
					//printf("%s %d %s %f\n","g:",g,"avgLowTime",avgLowTime);
					//printf("%s %d %s %f\n","numOps:",numOps,"numOpAvgEvalTime[numOps]:",numOpAvgEvalTime[numOps]);
				}
			}
		}
		
	}
	
	//Find averages
	
	for(h=0;h<maxNumInputs;++h){
		inputAvgRunTime[h] /= (double)numInputSets;
		inputAvgEvalTime[h] /= (double)numInputSets;
		//printf("%s %f\n", "numInputSets", (double)numInputSets);
		//printf("%s %d %s %f\n","h:",h,"inputAvgRunTime[h]", inputAvgRunTime[h]);
		//printf("%s %d %s %f\n","h:",h,"EvalAvgRunTime[h]", inputAvgEvalTime[h]);
	}
	
	int maxNumOps = 0;
	i=0;
	while(numOpCount[i++]!=0)
		maxNumOps=i;
	printf("%s %d\n","MaxNumOps:",maxNumOps);
		
	for(i=0;i<maxNumOps;++i){			
		if(numOpCount[i] != 0){
			numOpAvgRunTime[i] /= numOpCount[i];
			numOpAvgEvalTime[i] /= numOpCount[i];
			//printf("%s %f\n","numOpCount[i]", numOpCount[i]);
			//printf("%s %d %s %f\n","numOps:",i,"numOpAvgRunTime[i]", numOpAvgRunTime[i]);
		}
		else{
			printf("Dividing by 0\n");
		}
		
	}
			
	//Print out CSV Files
	
	for(i=0;i<maxNumInputs; ++i){ 
		fprintf(ofp1,"%d, %d, %lf, %lf\n", inArr[i], numOpArr[i],inputAvgRunTime[i],inputAvgEvalTime[i]);
		printf("%s %d\n", "i:", i);
		printf("%s %d\n", "inArr[i]:", inArr[i]);
		printf("%s %d\n", "numOpArr[i]:", numOpArr[i]);
		printf("%s %d\n", "inputAvgRunTime[i]:", inputAvgRunTime[i]);
		printf("%s %d\n", "inputAvgEvalTime[i]:", inputAvgEvalTime[i]);
	}
	
	fprintf(ofp2,"%s","Testing");
	
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
		
		
		superJsonToCSV(input, ofp1, ofp2, maxNumInputs, numInputSets, m, k, inputSize);

		free(input);
		fclose(ofp1);
		fclose(ofp2);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
