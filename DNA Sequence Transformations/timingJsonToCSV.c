/*
* timingJsonToCSV.c
*
* Takes JSON timing information file from DNASequence Transformation and codonOptimizer timing test and converts it to a csv file
* containing the columns:
* Number Of Bases, Average Transform Time
*
* Parameters:
*	[input json file][output csv][maxNumInputs][m number of runs][lowest k values to be averaged]
*
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

int timingJsonToCSV(char * input, FILE * ofp, int maxNumInputs, int m, int k){
	int i,j,h,g;
	int end = 0;
	int inArr[maxNumInputs];
	int numTimeMatches = maxNumInputs * m;
	int status;
	regex_t re;
	regmatch_t inputMatch;
	
	char *inputPattern = "[0-9]+ bases"; 
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
		while((c = input[(loc++)+end]) != ' '){
			numBuf[a++] = c;
		}
		numBuf[a] = '\x00';
		inArr[i] = atoi(numBuf);
		end += inputMatch.rm_eo;
		
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
	
	double inputAvgEvalTime [maxNumInputs];

	for(i=0;i<maxNumInputs;++i){
		inputAvgEvalTime[i] = 0;
	}
	
	double avgTime = 0;
	double lowTime[k];
	for(h=0;h<maxNumInputs;h++){
		double hiloTime = DBL_MAX;
		int numStored = 0;
		int hiloPos = 0;
		double time;
		for(j=0; j<m; ++j){
			if((time = timeArr[h*m+j]) < hiloTime){
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
		inputAvgEvalTime[h]= lowSum/(double)k;
	}
	
	
			
//Print out CSV Files
	
	//Print out column headers 
	fprintf(ofp,"%c%s,%s\n",'#', "Number Of Bases","Average Transform Time");
	
	//Print out data
	for(i=0;i<maxNumInputs; ++i){
		fprintf(ofp,"%d,%f\n", inArr[i],inputAvgEvalTime[i]);

	}
	
	return 0;
	
}
	
	
	
int main (int argc, char * argv[]){ //[input json file] [output csv] [maxNumInputs] [m number of runs] [lowest k values to be averaged] 

	if(argc == 6){

		FILE * ofp = fopen(argv[2], "w");
		int maxNumInputs = atoi(argv[3]);
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
		
		timingJsonToCSV(input, ofp, maxNumInputs, m, k);
		free(input);
		fclose(ofp);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
