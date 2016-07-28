/*
 * 
 * SamToMultiFasta.c
 * 
 * Converts SAM file to MultiFasta format, removing header tags and keeping the first SAM field as the header line
 * 
 * Parameters:
 * [Input SAM File][Output MultiFasta File Name] Optional: [Number of runs]
 * 
 * 
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "jsonData.h"

int samToMultiFasta(const char* input, char * output, long inputsize){

	int i = 0;
	while(input[i] == '@'){ // Remove header tags
		while(input[i] != '\n')
			i++;
		++i;
	}

	int numtabs;
	output[0] = '>';
	int k = 1;
	while(i < inputsize){

		numtabs = 0;

		while(input[i] != '\t'){ // Read in first field from SAM file as header to output MultiFile
			output[k++] = input[i++];
		}
		output[k++] = '\n';
		while(numtabs<9) // Skip over other tags until sequence is reached
			if(input[i++] == '\t')
				++numtabs;

		while(input[i] != '\t') //Copy sequence into MultiFile
			output[k++] = input[i++];

		while(input[i++] != '\n');
		
		if(input[i]!= '\x00'){
		output[k++] = '\n';

		output[k++] = '>';
		}
	}

	return k;

}

int main(int argc, char *argv[]){ //[Input SAM File][Output MultiFasta File Name] Optional: [Number of runs]
	if(!(argc == 3||argc == 4)){
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
	double *times ;
	int runs = 0;
	if(argc == 3){
		outputsize = samToMultiFasta(input,output,inputsize);
	}
	if(argc == 4){	//if a number of runs is given but no number of minimum times, default number of min times is 3
		runs = atoi(argv[3]);
		times = calloc(runs, sizeof(double)); 
		struct timeval time0, time1; 
		int i;
		for(i=0;i<runs;i++){ // Record time of each run
			gettimeofday(&time0,NULL);
			outputsize = samToMultiFasta(input,output,inputsize);
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
		printf("Error creating output file");
		return -1;
	}
	else{
		fwrite(output, 1, outputsize, ofp);
		fclose(ofp);
	}
	free(input);
	free(output);
	return 0;
 }
