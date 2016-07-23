/*
 * FastaTo4Bit.c
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
#include "jsonData.h"

int fastaTo4Bit(const char * input,char * output,long inputsize, long * outputsize){
	int i = 0;
	int k = 0;

	while((output[k++] = input[i++])!= '\n');// Remove first line

	while(i < inputsize){
		char byt;
		
		while(!(input[i] & '\x40')) ++i;
		if(i < inputsize){
			byt = input[i++]& '\x0f';
			byt <<= 4;
		}else break;

		while(!(input[i] & '\x40')) ++i;	

		if(i < inputsize){
			byt = byt|(input[i++] & '\x0f');
		}
		output[k++] = byt;
			
	}

	*outputsize = k;
	return 0;
}
int readInputToBuffer(FILE * ifp, char ** input, long * inputsize){
 	// Go to the end of the file //
	if(fseek(ifp, 0L, SEEK_END)== 0){
		// Get the size of the file. //
		*inputsize = ftell(ifp);
		if (*inputsize == -1) {
			fputs("Error finding size of file", stderr);
		return -1;
		 }
		//Allocate our buffer of that size +1 for null termination. //
		*input = malloc (sizeof(char) * ((*inputsize)+1));
		
		// Return to start of file //
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
			return -1;
		}
		//Read the entire file into memory//
		size_t newLen = fread(*input, sizeof(char), *inputsize, ifp);
		if(newLen == 0){
			fputs("Error reading file", stderr);
			return -1;
		} else {
			// Null termination character at the end of the input buffer //
			(*input)[newLen++] = '\0'; 
		}
		return 0;
	} else return -1;
}


int main(int argc, char *argv[]){
	if(!(argc == 3||argc == 4)){
		printf("Incompatible number of arguments\n");
                return -1;
        } 

	// Create Input Memory Buffer //
	char *input = NULL;
	FILE *ifp = fopen(argv[1],"r");
	long inputsize = 0;
	if(ifp != NULL){
		readInputToBuffer(ifp,&input,&inputsize);		
		fclose(ifp);
	}else{
		printf("%s\n", "the input file given does not exist");
		return 1;
	}

		// Create Output Buffer;
	char * output = malloc(sizeof(char)* (inputsize + 1));
	long outputsize = 0;
	
	double *times;
	int runs = 0;
	if(argc == 3){
		fastaTo4Bit(input,output,inputsize,&outputsize);
	}

	if(argc == 4){ //if the number of runs is given
		runs = atoi(argv[3]);
    times = calloc(runs, sizeof(double));
    struct timeval time0, time1; 
    int i;
    for(i=0;i<runs;i++){ // Record time of each run
    	gettimeofday(&time0,NULL);
    	fastaTo4Bit(input,output,inputsize, &outputsize);
    	gettimeofday(&time1,NULL);
    	times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
    }

		//timing.json file output generated //
		char *labelArr[1];
		labelArr[0] = "Transform Times";
		int numLabels = sizeof(labelArr)/sizeof(char*); 
		if(write_time_file(&times, labelArr, numLabels, runs) < 0)
			printf("error writing time file\n");
		free(times);
	}
	// Writing output buffer to specified output file//
	FILE *ofp = fopen(argv[2],"w");
	if(ofp == NULL){
		printf("Error creating output file\n");
		return -1;
        }else{
		fwrite(output, 1, outputsize, ofp);
		fclose(ofp);
	}
	free(input);
	free(output);
}
