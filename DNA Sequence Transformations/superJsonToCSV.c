/*
*superJsonToCSV.c
*
 Takes JSON timing information file from a superOptimizer timing test and converts it to 2 csv files containing the columns:
* CSV 1 (Inputs): Number Of Inputs, Average Run Time, Average Eval Time
* CSV 2 (Operations): Number Of Operations, Average Run Time, Average Eval Time
*
* Parameters:
*	[input json file] [output csv filename1][output csv filename2][maxNumInputs][numInputSets][number of superOpt runs]
*	[number of evaluation runs][lowest k values to be averaged] 
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

/*function applies specifically to json files made by superOptimizer, creates csv with data, averages the fastest runs to find a reasonable estimate for the cpu time spent*/
int superJsonToCSV(char * input, FILE * ofp1, FILE* ofp2, int maxNumInputs, int numInputSets, int superOptRuns, int evalRuns, int k, long inputSize){
	int i,j,h,g;
	int end = 0;
	int inArr[maxNumInputs];
	int numRuns = superOptRuns + evalRuns;
	double numTimeMatches = maxNumInputs * numInputSets * numRuns;
	int status;
	regex_t re;
	regmatch_t inputMatch;
	
	char *inputPattern = "[0-9]+ input"; 
	/*Number of Inputs*/
	
	if(regcomp(&re, inputPattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}
	
	//populates inArr with the number of inputs in each input set
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
		
		numBuf[a] = '\x00'; //makes array have string format so atoi works
		inArr[i] = atoi(numBuf);
		end += inputMatch.rm_eo;//increments end so that we begin our regexec at the end of the last match found
	}
	
	regfree(&re);
	
	/*Time Data (RunTime and Eval Time Alternating)*/
	
	regmatch_t timingMatch;
//	printf("%s %d\n","NumRuns:", numRuns);
//	printf("%s %d\n","numTimeMatches:", numTimeMatches);
	double *timeArr = malloc(numTimeMatches* sizeof(double));
	char *timePattern = "Run [0-9]+";
	
	if(regcomp(&re, timePattern, REG_EXTENDED) != 0){
		printf("Regex Comparison Error");
		return -1;
	}	
	
	end=0; //reset end
	//populates timeArr with timing data found in the json file
//	printf("%s %f", "numTimeMatches:", numTimeMatches);
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
		if(i == 8925){
			printf("%s %f", "timeArr[i]:", timeArr[i]);
		}
		end += timingMatch.rm_eo;
	}
	
	regfree(&re);

	/*NumOps*/
	int metaNumOps = maxNumInputs*numInputSets;
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
	
	//populates numOpArr with the number of operations that were found for each input set
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
	}
	
	regfree(&re);
	
	double numOpAvgRunTime [metaNumOps];
	double numOpAvgEvalTime [metaNumOps];
	double inputAvgRunTime [maxNumInputs];
	double inputAvgEvalTime [maxNumInputs];
	double numOpCount[metaNumOps];
	
	for(i=0;i<metaNumOps;++i){
		numOpCount[i]=0;
		numOpAvgEvalTime[i] = 0;
		numOpAvgRunTime[i] = 0;
	}
	for(i=0;i<maxNumInputs;++i){
		inputAvgRunTime[i] = 0;
		inputAvgEvalTime[i] = 0;
	}
	double avgTime = 0;
	double lowTime[k];
	
	//finds the fastest k runs for each input set and finds their average, does the same for number of inputs and time.
	for(h=0;h<maxNumInputs;h++){
		for(i=0;i<numInputSets; ++i){
			for(g=0; g<2; ++g){
				double hiloTime = DBL_MAX;
				int numStored = 0;
				int hiloPos = 0;
				double time;
				int runsVar = 0;
				if(g==0) runsVar = superOptRuns;
				else runsVar = evalRuns;
				for(j=0; j<runsVar; ++j){
//					printf("%s %d,%d,%d,%d\n","h,i,g,j ", h,i,g,j);
					if((time = timeArr[h*numInputSets*numRuns + i*numRuns + j + g*superOptRuns]) < hiloTime){
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
					inputAvgRunTime[h] += avgLowTime;
				}else{ // EvalTimes Avg - running sum
					int numOps = numOpArr[h*numInputSets+i];
					numOpAvgEvalTime[numOps-1] += avgLowTime;			
					inputAvgEvalTime[h] += avgLowTime;

				}
			}
		}
		
	}
	
	free(timeArr);
	
	//Find averages across input sets for each number of inputs
	
	for(h=0;h<maxNumInputs;++h){
		inputAvgRunTime[h] /= (double)numInputSets;
		inputAvgEvalTime[h] /= (double)numInputSets;
	}
	
	int maxNumOps = 0;
	i=0;
	while(numOpCount[i++]!=0)
		maxNumOps=i;
	printf("%s %d\n","MaxNumOps:",maxNumOps);
	
	//averages timing data with respect to number of operations to solve
	for(i=0;i<maxNumOps;++i){			
		if(numOpCount[i] != 0){
			numOpAvgRunTime[i] /= numOpCount[i];
			numOpAvgEvalTime[i] /= numOpCount[i];
		}
		else{
			printf("Dividing by 0\n");
		}
		
	}
			
//Print out CSV Files
	
	//Print out column headers 
	fprintf(ofp1,"%c%s,%s,%s\n",'#', "Number Of Inputs","Average Run Time", "Average Eval Time");

	//Print out data
	for(i=0;i<maxNumInputs; ++i){ 
		fprintf(ofp1,"%d,%f,%f\n", inArr[i],inputAvgRunTime[i],inputAvgEvalTime[i]);
	}
	
	//Print out column headers 
	fprintf(ofp2,"%c%s,%s,%s\n",'#', "Number Of Operations","Average Run Time", "Average Eval Time");
	
	//Print out data
	for(i=0;i<maxNumOps; ++i){ 
		fprintf(ofp2,"%d,%f,%f\n", i+1,numOpAvgRunTime[i],numOpAvgEvalTime[i]);
	}

	return 0;
	
}
	
	
	
int main (int argc, char * argv[]){ //[input json file] [output csv filename1] [output csv filename2] [maxNumInputs] [numInputSets] [number of superOpt runs][number of evaluation runs] [lowest k values to be averaged] 

	if(argc == 9){

		FILE * ofp1 = fopen(argv[2], "w");
		FILE * ofp2 = fopen(argv[3], "w");
		int maxNumInputs = atoi(argv[4]);
		int numInputSets = atoi(argv[5]);
		int superOptRuns = atoi(argv[6]);
		int evalRuns = atoi(argv[7]);
		int k = atoi(argv[8]);
		
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
		
		superJsonToCSV(input, ofp1, ofp2, maxNumInputs, numInputSets, superOptRuns, evalRuns, k, inputSize);

		free(input);
		fclose(ofp1);
		fclose(ofp2);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
