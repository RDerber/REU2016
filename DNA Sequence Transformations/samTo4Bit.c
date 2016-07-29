/*
 *
 * samTo4Bit.c 
 *
 * Functional for SAM files
 *
 * Includes an optional timing report exported in JSON format
 
 * Parameters:
 * [SAM input file] [Output 2Bit File Name] [Header Output File Name]Optional: [int number of runs]
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
 * Each nucleotide base will be converted to a four bit sequence:
 *		A = 0001
 *		C = 0011
 *		G = 0111
 *		T = 0100
 *		N = 1110
 *
 * The bases are then stored in bytes, retaining the left-to-right order in which they were read
 *  Ex: AG will convert to 0001 0111 
 *	
 * If the number of bases is not a multiple of 2, the last byte in the translation will end with (num bases % 2) pairs of zeros
 * 	These pairs of zeros will then translate back into FASTA as an extra '@' appended on the end of the sequence 
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "jsonData.h"

int samTo4Bit(const char* input, char * output, char * headers,  long inputsize, long * outputsize, long * headersize){

	int i = 0;
	while(input[i] == '@'){				//Skip over heading tag information
		while(input[i] != '\n')
			++i;
		++i;
	}

	int numtabs;
	int k = 0;
	int h = 0;
	while(i < inputsize){

		numtabs = 0;

		while(input[i] != '\t'){ 		// Put headers into null separated header array
			headers[h++] = input[i++];
		}
		headers[h++] = '\x00';
		while(numtabs<9)
			if(input[i++] == '\t')
				++numtabs;

		while(input[i] != '\t'){		//Convert sequence and write it
			char byt = input[i++];
			byt = byt & '\x0f';
			byt <<= 4;
			if(input[i] != '\t'){
				byt = byt | (input[i++] & '\x0f');
			}
			output[k++] = byt ;
		}

		while(input[i++] != '\n');
		
		if(input[i]!= '\x00'){
			output[k++] = '\n';
		}
	}
	*outputsize = k;
	*headersize = h;
	return 0;

}

int main(int argc, char *argv[]){	//arguments: [inputFile][outputFile][Header Output File Name] optional:[numberOfTestRuns]
	if(!(argc == 5||argc == 6)){
		printf("Incompatible number of arguments for SamTo4Bit\n");
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
	long outputsize = 0;
	long headersize = 0;
	
	double *times ;
	int runs = 0;
	if(argc == 4){
		samTo4Bit(input,output,headers,inputsize,&outputsize,&headersize);
	}

	if(argc == 5){ //if both number of runs and the number of minimum times is given
		runs = atoi(argv[4]);
		times = calloc(runs, sizeof(double));
		struct timeval time0, time1; 
    int i;
    for(i=0;i<runs;i++){ // Record time of each run
    	gettimeofday(&time0,NULL);
      samTo4Bit(input,output,headers,inputsize,&outputsize,&headersize);
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
	FILE *hfp = fopen(argv[3],"w");
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
	
	free(input);
	free(output);
	free(headers);
	return 0;
 } 
