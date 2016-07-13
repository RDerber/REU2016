/*
 * MultiFastaTo2Bit.c
 *
 *Inputs:
 * [FASTA input file] [Name of 2-bit output file]
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "jsonData.h"

int multiTo2Bit(const char * input,char * output,char * headers, int * positions, long inputsize, long * outputsize, long * headersize, long * positionsize){
	int i = 0;
	int k = 0;
	int h = 0;
	int j = 0;
	int p = 0;

	while(i < inputsize){
		char byt;

		if(input[i] == '>'){
			positions[p++] = k; 
			positions[p++] = -1; 
			while(input[i] != '\n')
				headers[h++] = input[i++];
			++i;
			headers[h++] = '\x00';
		}
		if(i < inputsize && input[i] != '>' && (input[i] & '\x60')){
			byt = input[i++]& '\x06';
			byt <<= 5;
		}else{
			++i;
			 continue;
		}

		for(j=2;j>0;--j){
			while(!(input[i] & '\x60')) ++i;
			if(i < inputsize && input[i]!= '>'){
				char temp = input[i++];
				temp&='\x06';
				temp<<=j*2-1;
				byt|=temp;
			}else break;
		}
		while(!(input[i] & '\x60')) ++i;	

		if(i < inputsize && input[i] != '>'){
			char temp = input[i++] & '\x06';
			temp>>=1;
			byt|=temp;
		}
		output[k++] = byt;
			
	}

	*outputsize = k;	
	*headersize = h;
	*positionsize = p;

	return 0;
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
	int * positions = malloc(sizeof(int) * inputsize);  
	long outputsize = 0;
	long headersize = 0;
	long positionsize = 0;
	
	double *times;
	int runs = 0;
	if(argc == 5){
		multiTo2Bit(input,output,headers,positions,inputsize,&outputsize,&headersize, &positionsize);
	}
	if(argc == 6){	//if a number of runs is given but no number of minimum times, default number of min times is 3
		runs = atoi(argv[5]);
		times = calloc(runs, sizeof(double)); 
		struct timeval time0, time1; 
		int i;
		for(i=0;i<runs;i++){ // Record time of each run
			gettimeofday(&time0,NULL);
			multiTo2Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
			gettimeofday(&time1,NULL);
			times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		}

	}
	
	// JSON timing.txt file output if [runs] and [num min times] arguments are included // 
	if(argc > 5){
		long numBases = outputsize*4;
		char *labelArr[1];
		labelArr[0] = "Transform Times";
		int numLabels = sizeof(labelArr)/sizeof(char*);
		
		if(write_time_file(&times, labelArr, numLabels,runs) < 0)
			printf("error writing time file\n");
		free(times);
	}
	// Writing output buffer to specified output file//
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
