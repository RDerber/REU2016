/*
 * FastqTo2Bit.c
 *
 * For regular FASTQ Files with a single sequence or multiple sequences (multiFasta)
 *
 * Includes an optional timing report exported in JSON format
 *  
 * Parameters:
 * [FASTQ input file] [Output 2Bit File Name] [Header Output File Name][Position Output File Name] [Quality Output File Name] Optional: [int number of runs]
 *		
 *	  Names for the output files. *These do not need to be created before hand* 
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

int fastqTo2Bit(const char * input,char * output,char * headers, int * positions, char* quality, long inputsize, long * outputsize, long * headersize, long * positionsize, long * qualitysize){
	int i = 0; // input buffer index
	int k = 0; // output buffer index
	int h = 0; // header buffer index
	int p = 0; // position buffer index
	int q = 0; // quality buffer index
	int j = 0;

	while(i < inputsize){
		char byt;
		
		// Move quality lines into quality buffer //
		if(input[i] == '+'){
			while(input[i++] != '\n'); //Remove secondary header line
			
			while(i<inputsize && input[i] != '\n' ){
				quality[q++] = input[i++];
			}
			++i;
			// Quality delimited with newline characters //
			quality[q++] = '\n';		
		
		
		}	
		
		// Move sequence header into header buffer, store its position //
		if(i < inputsize){
			if (input[i] == '@'){
				positions[p++] = k; 
				// header positions delimited with -1 //
				positions[p++] = -1; 
				while(input[i] != '\n' && i < inputsize)
					headers[h++] = input[i++];
				++i;
				// Headers delimited with Null characters // 
				headers[h++] = '\x00';
			}
		}else break;
		
		while(!(input[i] & '\x60'))++i;
		
		// Check for buffer overflow and if character is not a desired letter, increase to the next character in the buffer //
		if(input[i] != '@' && input[i] != '+'){
			
		//2Bit Conversion of the first base in the grouping of 4 //
			byt = input[i++]& '\x06';						
			byt <<= 5;
			
		}else{
			 continue;
		}

		//2Bit Conversion of the second and third bases of a grouping of 4 //
		for(j=2;j>0;--j){
			
			//Checking for desired character //
			while(!(input[i] & '\x60')) ++i;
			
			//Checking for buffer overflow and sequence headers //
			if(i < inputsize && input[i] != '@' && input[i] != '+'){
				char temp = input[i++];
				temp&='\x06';
				temp<<=j*2-1;
				byt = byt|temp;
			}else break;
		}
		
		
		//Checking for desired character //
		while(!(input[i] & '\x60')) ++i;	

		//Checking for buffer overflow and sequence headers //
		if(i < inputsize && input[i] != '@' && input[i] != '+'){
			
			//2 Bit Conversion of the final base in a group of 4
			char temp = input[i++] & '\x06';
			temp>>=1;
			byt= byt|temp;
		}
		output[k++] = byt;
			
	}
	//set sizes of each ouputFile buffer before returning //
	*outputsize = k;	
	*headersize = h;
	*positionsize = p;
	*qualitysize = q;

	return 0;
}

int main(int argc, char *argv[]){ // [input][output][headerfile][positionfile][qualityfile][numruns]
	if(!(argc == 6||argc == 7)){
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

	//Create outputFile Buffers:
	//output buffer (2bit encoded characters),
	//header buffer (null delimited header lines)
	//position buffer (positions of start of each sequence, delimited with -1)
	//quality buffer (newLine delimited quality information - ASCII characters ! through ~)
	char * output = malloc(sizeof(char)* (inputsize + 1));
	char * headers = malloc(sizeof(char)* (inputsize + 1));
	int * positions = malloc(sizeof(int) * inputsize);  
	char * quality = malloc (sizeof(int) * (inputsize + 1));
	long outputsize = 0;
	long headersize = 0;
	long positionsize = 0;
	long qualitysize = 0;
	
	double *times;
	int runs = 0;
	if(argc == 6){
		fastqTo2Bit(input,output,headers,positions,quality,inputsize,&outputsize,&headersize,&positionsize,&qualitysize);
	}
	// if [runs] argument is included, activate timing code //
	if(argc == 7){	
		runs = atoi(argv[6]);
		times = calloc(runs, sizeof(double)); 
		struct timeval time0, time1; 
		int i;
		// Record time of each run //
		for(i=0;i<runs;i++){ 
			gettimeofday(&time0,NULL);
			fastqTo2Bit(input,output,headers,positions,quality,inputsize,&outputsize,&headersize,&positionsize,&qualitysize);
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
	FILE *qfp = fopen(argv[5],"w");
	if(ofp == NULL){
		printf("Error creating output file\n");
		return -1;
        }else if(hfp == NULL){
		printf("Error writing header file\n");
		return -1;
	}else if(pfp == NULL){
		printf("Error writing positions file\n");
		return -1;
	}else if(qfp == NULL){
		printf("Error writing quality file\n");
		return -1;
	}else{
		fwrite(output, sizeof(char), outputsize, ofp);
		fwrite(headers, sizeof(char), headersize, hfp);
		fwrite(quality, sizeof(char), qualitysize, qfp);
		int i;
		for(i=0; i<positionsize; ++i){
			fprintf(pfp,"%d",positions[i]);
		}
		fclose(ofp);
		fclose(hfp);
		fclose(pfp);
		fclose(qfp);
		}
		
	free(quality);
	free(positions);
	free(headers);
	free(input);
	free(output);
}
