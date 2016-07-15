/*
 * samTo2Bit.c 
 *
 * Functional for SAM files
 *
 * Includes an optional timing report exported in JSON format
 
 * Parameters:
 * [SAM input file] [Output 2Bit File Name] [Header Output File Name][Position Output File Name] Optional: [int number of runs]
 *
 *			Names for the output files. *These do not need to be created before hand* 
 * 
 * Output file (2bit encoded characters)
 * Header file (null delimited header lines)
 * Position file (positions, in bytes, of each sequence in the 2bit file, delimited with -1)
 *
 * Number of runs is optional
 *		 Number of runs must be provided as the 3rd argument to recieve a timing report
 *
 * The current version has the following settings:
 *		-omits heading tag information
 *		-reads in the QNAME field of the SAM file and saves it to a null delimited headerfile
 *		-omits all other field information
 *		-writes out each sequence in a multi-fasta 2 bit format: 
 *				>writes sequences to output 2bit file
 *				>writes starting position of each sequence to the position output file 
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
 */

#include <stdio.h>
#include <stdlib.h>
#include "jsonData.h"

long samTo2Bit(const char* input, char * output, char * headers, int* positions,  long inputsize, long * outputsize, long * headersize, long * positionsize){

	int i = 0;
	while(input[i] == '@'){				// Skip over heading tag information //
		while(input[i] != '\n')
			++i;
		++i;
	}

	int numtabs;
	int numBases = 0;
	int k = 0;
	int h = 0;
	int p = 0;
	while(i < inputsize){

		numtabs = 0;

		while(input[i] != '\t'){ 		// Put headers (QNAME fields) into null separated header array //
			headers[h++] = input[i++];
		}
		headers[h++] = '\x00';
		positions[p++] = k;
		positions[p++] = -1;
		while(numtabs<9)
			if(input[i++] == '\t')
				++numtabs;

		while(input[i] != '\t'){		//Convert sequences to 2Bit and store in output buffer //
			char byt = input[i++];
			numBases++;
			byt &= '\x06';
			byt <<= 5;
			int j;
			for(j=2;j>0;--j){
				if(input[i] != '\t'){
					char temp = (input[i++] & '\x06');
					numBases++;
					temp <<= (j*2) - 1;
					byt |= temp;
				}
			}
			if(input[i] != '\t'){
				char temp = (input[i++] & '\x06');
				numBases++;
				temp >>= 1;
				byt |= temp;
			}
			output[k++] = byt ;
		}
		
		// remove quality (FASTQ) information at end of line //
		while(input[i++] != '\n');
		
	}
	
//set sizes of each ouputFile buffer before returning //
	*outputsize = k;
	*headersize = h;
	*positionsize = p; 
	return numBases;

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


int main(int argc, char *argv[]){	//arguments: [inputFile][outputFile][headerFile][positionFile] optional:[numberOfTestRuns]
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
		char * output = malloc(sizeof(char)* (inputsize));
	char * headers = malloc(sizeof(char)* (inputsize));
	int * positions = malloc(sizeof(int) * inputsize);
	long outputsize = 0;
	long headersize = 0;
	long positionsize = 0;
	double *times ;
	int runs = 0;
	long numBases = 0;
	if(argc == 5){
		numBases = samTo2Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
	}
	// activate timing if the [runs] argument is included // 
	if(argc == 6){	
		runs = atoi(argv[5]);
		times = calloc(runs, sizeof(double)); 
		struct timeval time0, time1; 
		int i;
		 // Record time of each run //
		for(i=0;i<runs;i++){
			gettimeofday(&time0,NULL);
			numBases = samTo2Bit(input,output,headers,positions,inputsize,&outputsize,&headersize,&positionsize);
			gettimeofday(&time1,NULL);
			times[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
		}

	// timing.json output file output generated //
		char *labelArr[1];
		labelArr[0] = "Transform Times";
		int numLabels = sizeof(labelArr)/sizeof(char*); 
	
		if(write_time_file(&times, labelArr, numLabels, runs) < 0)
			printf("error writing time file\n");
		free(times);
	}
	
	// Writing output buffers to specified output files//
	
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
