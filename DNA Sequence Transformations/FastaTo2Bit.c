/*
 * FastaTo2Bit.c
 *
 * Functional for regular FASTA Files with a single sequence
 *
 * Includes an optional timing report exported in JSON format
 *  
 * Parameters: 
 * [Input FASTA File Name] [Output 2Bit File Name] [int number of runs]
 *
 * Number of runs is optional
 *		 Number of runs must be provided as the 3rd argument to recieve a timing report
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
#include "jsonData.h"
/*
* Reads in A,C,G,T characters from input buffer, converts to 00, 01, 11, and 10 respectively, and writes to output buffer
*@param input buffer
*@param output buffer
*@param size of input buffer
*@return size of the output buffer
*/
int twoBit(char * input, char * output, long inputsize, long * numBases){ 
	
	// Move header line into output buffer // 
	size_t firstlnlen;
	for(firstlnlen = 0; input[firstlnlen] != '\n'; ++firstlnlen){ 
		output[firstlnlen] = input[firstlnlen];
	}
	output[firstlnlen]='\n';
	
	
	long i = firstlnlen + 1;	// input buffer index
	long k = i-1;			// output buffer index
	
	// Read from input buffer, translate, and write to output buffer // 
	while(i < inputsize){

		while(i<inputsize && !(input[i]&'\x40')){ // Check for buffer overflow 
			++i;			// If character is not a desired letter, increase to the next character in the buffer
		}
		if(i >= inputsize) break;		// If index increases past buffer size, stop reading
		char byt = input[i];
		++k;
		++i;
		byt = byt &'\x06';	// 2 Bit conversion of first base in grouping of 4
		byt = byt << 5;		// Shift first base to the left-most position on the output byte
		char temp;
		unsigned j;
		for(j = 2; j > 0 && i < inputsize; --j){ // 2 Bit conversion of the 2nd and 3rd bases

			if(input[i] & '\x40'){
				temp = input[i];
				++i;
				temp = temp & '\x06';	//convert base character to 2Bit
				temp = temp << (j*2)-1;	//shift left corresponding to its order in the byte
				byt = byt|temp;
			} else {
				++j;		//reset j to previous value so place within byte is maintained
				++i;		//skip input value if not a character
			}
		}
		
		// Checking for buffer overflow and desired character //
		while(i < inputsize && !(input[i] & '\x40')){ 
			++i;
		}
		// 2 Bit conversion of the last base in a grouping of 4 //
		if(i < inputsize){	
			temp = input[i];
			++i;
			temp = temp & '\x06';
			temp = temp >> 1;
			byt = byt|temp;
		}

	output[k] = byt;

	}
	*numBases = 4*((k+1)-firstlnlen);
	return k+1; // returns the size of the output array 
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

	// Create Output Buffer //
	char * output = malloc(sizeof(char)* (inputsize+1));
	int outputsize = 0;
	
	double *times;
	int runs = 0;
	long numBases = 0;
	if(argc == 3){
		outputsize = twoBit(input,output,inputsize,&numBases);
	}
	//if [runs] argument is included, activate timing code //
	if(argc == 4){	
		runs = atoi(argv[3]);
		times = calloc(runs, sizeof(double)); 
		struct timeval time0, time1; 
		int i;
		// Record time of each run //
		for(i=0;i<runs;i++){ 
			gettimeofday(&time0,NULL);
			outputsize = twoBit(input,output,inputsize,&numBases);
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
