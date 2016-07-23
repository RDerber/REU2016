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

int timingJsonToCSV(char * input, FILE * ofp, int numKeys, int m, int k){
	int i,j,h,g;
	int end = 0;
	int inArr[numKeys];
	int numTimeMatches = numKeys * m * 2;
	int status;
	regex_t re;
	regmatch_t inputMatch;
	
	char *inputPattern = "Key Size"; 
	/*Number of Inputs*/
	
	if(regcomp(&re, inputPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}
	
	for(i=0; i<numKeys; ++i){
		status = regexec(&re, input+end, 1, &inputMatch, 0); // Find Num Inputs
		if(status != 0){
			printf("%s %s","The following expression was not found:", inputPattern);
			return -1;
		}
		int loc = inputMatch.rm_eo+3;
		char c;
		int a=0;
		char numBuf[20];
//		printf("%s %d\n","loc:",loc);
		while((c = input[(loc++)+end]) != ','){
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
	
	/*Time Data */
	
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
		while((c=input[(loc++)+end]) != ',' && c!='\n' && c!='\r'){
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00';
		timeArr[i] = atof(numBuf); 
		end += timingMatch.rm_eo;
	}
	
	regfree(&re);
	
	double inputAvgEvalTime [numKeys];
	double inputAvgRunTime [numKeys];

	for(i=0;i<numKeys;++i){
		inputAvgEvalTime[i] = 0;
		inputAvgRunTime [i] = 0;
	}
	
	double avgTime = 0;
	double lowTime[k];
	for(h=0;h<numKeys;h++){
		for(g=0; g < 2; ++g){
			double hiloTime = DBL_MAX;
			int numStored = 0;
			int hiloPos = 0;
			double time;
			for(j=0; j<m; ++j){
				//printf("%s %f\n", "hiloTime:", hiloTime);
				if((time = timeArr[h*m*2+g*m+j]) < hiloTime){
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
				inputAvgRunTime[h] += avgLowTime;
			}else{ // EvalTimes Avg - running sum		
				inputAvgEvalTime[h] += avgLowTime;
			}
		}
			
			
	}
	
	
			
	//Print out CSV Files
	for(i=0;i<numKeys; ++i){
		fprintf(ofp,"%d,%f,%f\n", inArr[i],inputAvgRunTime[i],inputAvgEvalTime[i]);
	//	fprintf(ofp2,"%d,%f\n", inArr[i],inputAvgEvalTime[i]);
	//	printf("%s %d\n", "i:", i);
	//	printf("%s %d, %p\n", "inArr[i]:", inArr[i], inArr);
	//	printf("%s %f, %p\n", "inputAvgRunTime[i]:", inputAvgRunTime[i], inputAvgRunTime);
	//	printf("%s %f, %p\n", "inputAvgEvalTime[i]:", inputAvgEvalTime[i], inputAvgEvalTime);
	}
	
	return 0;
	
}
	
	
	
int main (int argc, char * argv[]){ //[input json file] [output csv] [numKeys] [m number of runs] [lowest k values to be averaged] 

	if(argc == 6){

		FILE * ofp = fopen(argv[2], "w");
		int numKeys = atoi(argv[3]);
		int m = atoi(argv[4]);
		int k = atoi(argv[5]);
		
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
		
		timingJsonToCSV(input, ofp, numKeys, m, k);
		free(input);
		fclose(ofp);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
