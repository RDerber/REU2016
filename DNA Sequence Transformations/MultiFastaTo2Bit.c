/*
 * MultiFastaTo2Bit.c
 *
 * Functional for regular FASTA Files with a single sequence or multiple sequences (multiFasta)
 *
 * Includes an optional timing report exported in JSON format
 *  
 * Parameters:
 * [FASTA input file] [Output 2Bit File Name] [Header Output File Name][Position Output File Name] Optional: [int number of runs]
 *
 *		Names for the output files. *These do not need to be created before hand* 
 *
 * Output file (2bit encoded characters)
 * Header file (null delimited header lines)
 * Position file (positions, in bytes, of each sequence in the 2bit file, delimited with -1)
 * 
 * Number of runs is optional:
 *		- Number of runs must be provided as the 3rd argument to recieve a timing report
 *
 *
 * Each nucleotide base will be converted to a two bit character:
 *		A = 00
 *		C = 01
 *		G = 11
 *		T = 10
 *
 * The bases are then stored in bytes, retaining the left-to-right order in which they were read
 *  Ex: ACGT will convert to 0001 1110 
 *	
 * If the number of bases is not a multiple of 4, the last byte in the translation will end with (num bases % 4) pairs of zeros
 * 	These pairs of zeros will then translate back into FASTA as extra A's appended on the end of the sequence
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "jsonData.h"

int multiTo2Bit(const char * input,char * output,char * headers, int * positions, long inputsize, long * outputsize, long * headersize, long * positionsize){
	int i = 0; // input buffer index
	int k = 0; // output buffer index
	int h = 0; // header buffer index
	int p = 0; // position buffer index
	int j = 0;

	while(i < inputsize){
		char byt;
		// Move sequence header into header buffer, store its position //
		if(input[i] == '>'){
			positions[p++] = k; 
			// header positions delimited with -1 //
			positions[p++] = -1; 
			while(input[i] != '\n')
				headers[h++] = input[i++];
			++i;
			
			// Headers delimited with Null characters // 
			headers[h++] = '\x00';
		}
		// Check for buffer overflow and if character is not a desired letter, increase to the next character in the buffer //
		if(i < inputsize && input[i] != '>' && (input[i] & '\x60')){ 
			
		//2Bit Conversion of the first base in the grouping of 4 //
			byt = input[i++]& '\x06';						
			byt <<= 5;
			
		}else{
			++i;
			 continue;
		}
		//2Bit Conversion of the second and third bases of a grouping of 4 //
		for(j=2;j>0;--j){
			
			//Checking for desired character //
			while(!(input[i] & '\x60')) ++i;
			
			//Checking for buffer overflow and sequence headers //
			if(i < inputsize && input[i]!= '>'){
				char temp = input[i++];
				temp&='\x06';
				temp<<=j*2-1;
				byt|=temp;
			}else break;
		}
		//Checking for desired character //
		while(!(input[i] & '\x60')) ++i;	

		//Checking for buffer overflow and sequence headers //
		if(i < inputsize && input[i] != '>'){
			
			//2 Bit Conversion of the final base in a group of 4
			char temp = input[i++] & '\x06';
			temp>>=1;
			byt|=temp;
		}
		output[k++] = byt;
			
	}
	//set sizes of each ouputFile buffer before returning //
	*outputsize = k;	
	*headersize = h;
	*positionsize = p;

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

int main(int argc, char *argv[]){ // [input][output][headerfile][positionfile][num runs]
	if(!(argc == 5||argc == 6)){
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

	//Create outputFile Buffers:
	//output buffer (2bit encoded characters),
	//header buffer (null delimited header lines)
	//position buffer (positions of start of each sequence, delimited with -1) //
	char * output = malloc(sizeof(char)* (inputsize + 1));
	char * headers = malloc(sizeof(char)* (inputsize + 1));
	int * positions = malloc(sizeof(int) * inputsize);  
	long outputsize = 0;
	long headersize = 0;
	long positionsize = 0;
	
	double *times;
	int runs = 0;
	if(argc == 5){
		multiTo2Bit(input,output,headers,positions,inputsize,&outputsize,&headersize, &positionsize);
	}
	// if [runs] argument is included, activate timing code //
	if(argc == 6){	
		runs = atoi(argv[5]);
		times = calloc(runs, sizeof(double)); 
		struct timeval time0, time1; 
		int i;
		// Record time of each run //
		for(i=0;i<runs;i++){ 
			gettimeofday(&time0,NULL);
			multiTo2Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
			gettimeofday(&time1,NULL);
			times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		}

	//timing.json file output generated //
		long numBases = outputsize*4;
		char *labelArr[1];
		labelArr[0] = "Transform Times";
		int numLabels = sizeof(labelArr)/sizeof(char*);
		if(write_time_file(&times, labelArr, numLabels,runs) < 0)
			printf("error writing time file\n");
		free(times);
	}
	// Writing output buffers to specified output files//
	FILE *ofp = fopen(argv[2],"w");
	FILE *hfp = fopen(argv[3],"w");
	FILE *pfp = fopen(argv[4],"w");
	if(ofp == NULL){
		printf("Error creating output file\n");
		return -1;
        }else if(hfp == NULL){
		printf("Error writing header file\n");
		return -1;
	}else if(pfp == NULL){
		printf("Error writing positions file\n");
		return -1;
	}else{
		fwrite(output, sizeof(char), outputsize, ofp);
		fwrite(headers, sizeof(char), headersize, hfp);
		int i;
		for(i=0; i<positionsize; ++i){
			fprintf(pfp,"%d",positions[i]);
		}
		fclose(ofp);
		fclose(hfp);
		fclose(pfp);
		}

	free(positions);
	free(headers);
	free(input);
	free(output);
}
