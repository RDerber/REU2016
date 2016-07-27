/*
 * FastaTo4Bit.c
 *
 * Inputs:
 *  [FASTA input file][Name of 4-bit output file][number of runs]
 *
 * The last argument is optional:
 *	-If included, a timing report will be output in timeStats.txt
 *	
 * Each four bit sequence will be converted to a nucleotide base:
 *		A = 0001
 *		C = 0011
 *		G = 0111
 *		T = 0100
 *		N = 1110
 *    
 * The bases are then stored as ASCII characters, retaining the left-to-right order in which they were read
 *  Ex: AG will convert to 0001 0111
 *
 * If the number of bases is not a multiple of 2, the last byte in the translation will end with (num bases % 2) pairs of zeros
 * 	These pairs of zeros will then translate back into FASTA as an extra '@' appended on the end of the sequence
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "jsonData.h"

int fastaTo4Bit(const char * input,char * output,long inputsize, long * outputsize){
	int i = 0;
	int k = 0;

	while((output[k++] = input[i++])!= '\n');// Remove header (first) line

	while(i < inputsize){
		char byt;
		
		while(!(input[i] & '\x40')) ++i; // Check for desired base character
		if(i < inputsize){
			byt = input[i++]& '\x0f'; // Convert nucleotide base char to 4Bit sequence
			byt <<= 4;
		}else break;

		while(!(input[i] & '\x40')) ++i;	

		if(i < inputsize){
			byt = byt|(input[i++] & '\x0f'); // Convert second nucleotide base and append to byte
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


int main(int argc, char *argv[]){ //[FASTA input file][Name of 4-bit output file][number of runs]
	if(!(argc == 3||argc == 4)){
		printf("Incompatible number of arguments for FastaTo4Bit\n");
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

