#include <stdio.h>
#include <stdlib.h>
#include "writeJson.h"

int samTo4Bit(const char* input, char * output, char * headers, int* positions,  long inputsize, long * outputsize, long * headersize, long * positionsize){

	int i = 0;
	while(input[i] == '@'){				//Skip over heading tag information
		while(input[i] != '\n')
			++i;
		++i;
	}

	int numtabs;
	int k = 0;
	int h = 0;
	int p = 0;
	while(i < inputsize){

		numtabs = 0;

		while(input[i] != '\t'){ 		// Put headers into null separated header array
			headers[h++] = input[i++];
		}
		headers[h++] = '\x00';
		positions[p++] = k;
		positions[p++] = -1;
		while(numtabs<9)
			if(input[i++] == '\t')
				++numtabs;

		while(input[i] != '\t'){		//Convert sequence and write it
			char byt = input[i++];
			byt &= '\x06';
			byt <<= 5;
			int j;
			for(j=2;j>0;--j){
				if(input[i] != '\t'){
					char temp = (input[i++] & '\x06');
					temp <<= (j*2) - 1;
					byt |= temp;
				}
			}
			if(input[i] != '\t'){
				char temp = (input[i++] & '\x06');
				temp >>= 1;
				byt |= temp;
			}
			output[k++] = byt ;
		}

		while(input[i++] != '\n');
		
	}
	*outputsize = k;
	*headersize = h;
	*positionsize = p; 
	return 0;

}

int main(int argc, char *argv[]){	//arguments: [inputFile][outputFile][headerFile][positionFile] optional:[numberOfTestRuns][NumberOfRunsToAverage]
	if(!(argc == 5||argc == 6||argc == 7)){
		printf("Incompatible number of arguments\n");
		return -1;
	}
	// Create Input Memory Buffer //
	char *input = NULL;
	FILE *ifp = fopen(argv[1],"r");
	long inputsize = 0;
	if(ifp != NULL){
	 // Go to the end of the file //
	if(fseek(ifp, 0L, SEEK_END)== 0){
		// Get the size of the file. //

		inputsize = ftell(ifp);
		if (inputsize == -1) {
			fputs("Error finding size of file", stderr);
		 }

	
		//Allocate our buffer of that size +1 for null termination. //
		input = malloc (sizeof(char) * (inputsize+1));
		

		// Return to start of file //
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
		}

		//Read the entire file into memory//
		size_t newLen = fread(input, sizeof(char), inputsize, ifp);
		if(newLen == 0){
			fputs("Error reading file", stderr);
		} else {
			input[newLen++] = '\0'; // Null termination character at the end of the input buffer 
			}
		}			
		fclose(ifp);
	}else{
		printf("%s\n", "the input file given does not exist");
		return 1;
	}

		// Create Output Buffer;
	char * output = malloc(sizeof(char)* (inputsize));
	char * headers = malloc(sizeof(char)* (inputsize));
	int * positions = malloc(sizeof(int) * inputsize);
	long outputsize = 0;
	long headersize = 0;
	long positionsize = 0;
	
	float *times ;
	int runs = 0;
	int numTimes = 0;
	if(argc == 5){
		samTo4Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
	}
	if(argc == 6){	//if a number of runs is given but no number of minimum times, default number of min times is 3
		runs = atoi(argv[5]);
		numTimes = 3;
		times = calloc(runs, sizeof(float)); 
		struct timeval time0, time1; 
		int i;
		for(i=0;i<runs;i++){ // Record time of each run
			gettimeofday(&time0,NULL);
			samTo4Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
			gettimeofday(&time1,NULL);
			times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		}

	}
	if(argc == 7){ //if both number of runs and the number of minimum times is given
		runs = atoi(argv[5]);
                numTimes = atoi(argv[6]);
		times = calloc(runs, sizeof(float));
                struct timeval time0, time1; 
                int i;
                for(i=0;i<runs;i++){ // Record time of each run
                        gettimeofday(&time0,NULL);
                        samTo4Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
                        gettimeofday(&time1,NULL);
                        times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
                }

	}

	// JSON timing.txt file output if [runs] and [num min times] arguments are included // 
	if(argc > 5){ 
		if(write_time_file(times, runs, numTimes,inputsize) < 0)
			printf("error writing time file\n");
		free(times);
	}
	// Writing output buffer to specified output file//
	FILE *ofp = fopen(argv[2],"w");
	FILE *hfp = fopen(argv[3],"w");
	FILE *pfp = fopen(argv[4],"w");
	if(ofp == NULL){
		printf("Error creating output file");
		return -1;
	}
	else{
		fwrite(output, 1, outputsize, ofp);
		fclose(ofp);
	}

	if(hfp == NULL){
		printf("Error creating header file");
		return -1;
	}
	else{
		fwrite(headers, 1, headersize, hfp);
		fclose(hfp);
	}
	if(pfp == NULL){
		printf("Error creating position file");
		return -1;
	}
	else{	int j;
		for(j=0;j<positionsize;++j){
			fprintf(pfp,"%d",positions[j]); 
		}
	}
	
	free(input);
	free(output);
	free(headers);
	free(positions);
	return 0;
 } 
