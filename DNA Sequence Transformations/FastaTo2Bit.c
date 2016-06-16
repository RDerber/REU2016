/*
 * FastaTo2Bit.c
 *
 * Functional for regular FASTA
 *	Includes an optional timing report exported in JSON format
 *  
 * Parameters: [Input FASTA File Name] [Output 2Bit File Name] [int number of runs] [int number of minimum timing values to retain]
 *
 * Number of runs and min timing values are optional
 *		 Number of runs must be provided to recieve a timing report
 *		 The default number of min timing values is 3 if unsepecified.
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
#include <sys/time.h>
#include <float.h>
#include "writeJson.h"
/*
* Reads in A,C,G,T characters from input buffer, converts to 00, 01, 11, and 10 respectively, and writes to output buffer
*@param input buffer
*@param output buffer
*@param size of input buffer
*@return size of the output buffer
*/
int twoBit(char * input, char * output, long inputsize){ 
	
	// Move header line into output buffer // 
	size_t firstlnlen;
	for(firstlnlen = 0; input[firstlnlen] != '\n'; ++firstlnlen){ 
		output[firstlnlen] = input[firstlnlen];
	}
	output[firstlnlen]='\n';
	
	
	long i = firstlnlen + 1; // input buffer index
	long k = i-1;				// output buffer index
	
	// Read from input buffer, translate, and write to output buffer // 
	while(i < inputsize){

		while(i<inputsize && !(input[i]&'\x40')){ // Check for buffer overflow 
			++i;												// If character is not a desired letter, increase to the next character in the buffer
		}
		if(i >= inputsize) break;						// If index increases past buffer size, stop reading
		char byt = input[i];
		++k;
		++i;
		byt = byt &'\x06';								// 2 Bit conversion of first base in grouping of 4
		byt = byt << 5;									// Shifted to the left-most position on the output byte
		char temp;
		unsigned j;
		for(j = 2; j > 0 && i < inputsize; --j){ // 2 Bit conversion of the 2nd and 3rd bases

			if(input[i] & '\x40'){
				temp = input[i];
				++i;
				temp = temp & '\x06';
				temp = temp << (j*2)-1;
				byt = byt|temp;
			} else {
				++j;
				++i;
			}
		}
		while(i < inputsize && !(input[i] & '\x40')){ //Checking for buffer overflow and desired character
			++i;
		}

			
		if(i < inputsize){									// 2 Bit conversion of the last base in a grouping of 4
			temp = input[i];
			++i;
			temp = temp & '\x06';
			temp = temp >> 1;
			byt = byt|temp;
		}

	output[k] = byt;

	}
	return k+1; // returns the size of the output array 
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
	char * output = malloc(sizeof(char)* (inputsize+1));
	int outputsize = 0;
	
	float *times;
	int runs = 0;
	int numTimes = 0;
	if(argc == 3){
		outputsize = twoBit(input,output,inputsize);
	}
	if(argc == 4){	//if a number of runs is given but no number of minimum times, default number of min times is 3
		runs = atoi(argv[3]);
		numTimes = 3;
		times = calloc(runs, sizeof(float)); 
		struct timeval time0, time1; 
		int i;
		for(i=0;i<runs;i++){ // Record time of each run
			gettimeofday(&time0,NULL);
			outputsize = twoBit(input,output,inputsize);
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
                        outputsize = twoBit(input,output,inputsize);
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
