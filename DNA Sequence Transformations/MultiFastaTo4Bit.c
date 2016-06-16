/*
 * MultiFastaTo4Bit.c
 *
 *Inputs:
 * [FASTA input file] [Name of 4-bit output file][number of runs] [number of minimum time values to compare]
 *
 *The last two arguments are optional:
 *	-If included, a timing report will be output in timeStats.txt
 *	-If just the number of runs are provided, the num of min time values will default to 3
 *	
 *    
 *
 */





#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include "writeJson.h"

int multiTo4Bit(const char * input,char * output,char * headers, long inputsize, long * outputsize, long * headersize){
	int i = 0;
	int k = 0;
	int h = 0;

	while(i < inputsize){
		char byt;

		if(input[i] == '>'){
			output[k++] = '<';
			while(input[i] != '\n')
				headers[h++] = input[i++];
			++i;
			headers[h++] = '\x00';
		}
		
		while(!(input[i] & '\x60')) ++i;
		if(i < inputsize && input[i] != '>'){
			byt = input[i++]& '\x0f';
			byt <<= 4;
		}else continue;

		while(!(input[i] & '\x60')) ++i;	

		if(i < inputsize && input[i] != '>'){
			byt = byt|(input[i++] & '\x0f');
		}
		output[k++] = byt;
			
	}

	*outputsize = k;	
	*headersize = h;

	return 0;
}

int main(int argc, char *argv[]){
	if(!(argc == 3||argc == 4||argc == 5)){
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
	char * output = malloc(sizeof(char)* (inputsize + 1));
	char * headers = malloc(sizeof(char)* (inputsize + 1));
	long outputsize = 0;
	long headersize = 0;
	
	float *times;
	int runs = 0;
	int numTimes = 0;
	if(argc == 3){
		multiTo4Bit(input,output,headers,inputsize,&outputsize,&headersize);
	}
	if(argc == 4){	//if a number of runs is given but no number of minimum times, default number of min times is 3
		runs = atoi(argv[3]);
		numTimes = 3;
		times = calloc(runs, sizeof(float)); 
		struct timeval time0, time1; 
		int i;
		for(i=0;i<runs;i++){ // Record time of each run
			gettimeofday(&time0,NULL);
			multiTo4Bit(input,output,headers,inputsize,&outputsize,&headersize);
			gettimeofday(&time1,NULL);
			times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		}

	}
	if(argc == 5){ //if both number of runs and the number of minimum times is given
		runs = atoi(argv[3]);
                numTimes = atoi(argv[4]);
                times = calloc(runs, sizeof(float));
                struct timeval time0, time1; 
                int i;
                for(i=0;i<runs;i++){ // Record time of each run
                        gettimeofday(&time0,NULL);
                        multiTo4Bit(input,output,headers,inputsize, &outputsize, &headersize);
                        gettimeofday(&time1,NULL);
                        times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
                }

	}

	// JSON timing.txt file output if [runs] and [num min times] arguments are included // 
	if(argc > 3){
		if(write_time_file(times, runs, numTimes,inputsize) < 0)
			printf("error writing time file\n");
		free(times);
	}
	// Writing output buffer to specified output file//
	FILE *ofp = fopen(argv[2],"w");
	FILE *hfp = fopen("4BitHeaders.txt","w");
	if(ofp == NULL || hfp == NULL){
		printf("Error creating output file\n");
		return -1;
        }else{
		fwrite(output, 1, outputsize, ofp);
		fwrite(headers, 1, headersize, hfp);
		fclose(ofp);
	}
	free(headers);
	free(input);
	free(output);
}
