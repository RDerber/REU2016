/*
 * FastaTo2Bit.c
 *
 * Functional for regular FASTA
 *
 * we now get time, need to add for loop with minheap to get minimum times.
 *
 * 
 */



#include <stdio.h>
#include <stdlib.h>

int twoBit(char * input, char * output, long inputsize){
	size_t firstlnlen;
	for(firstlnlen = 0; input[firstlnlen] != '\n'; ++firstlnlen){
		output[firstlnlen] = input[firstlnlen];
//		printf("%c",input[firstlnlen]);
	}
	output[firstlnlen]='\n';
	long i = firstlnlen + 1;
//	printf("\n%c\n",input[i-1]);
	long k = i-1;
//	printf("%s %d %s %d \n","firstlnlen:",firstlnlen,"inputsize:",inputsize);
	while(i < inputsize){
//	printf("%s %d %s %c \n","i is:", i, "input[i] is:",input[i]);
//	printf("%d \n",i);
//	printf("%s %c \n","Input[i]:",input[i]);
		while(i<inputsize && !(input[i]&'\x40')){
			++i;
		}
		if(i >= inputsize) break;
		char byt = input[i];
		++k;
		++i;
		byt = byt &'\x06';
		byt = byt << 5;
		char temp;
		unsigned j;
		for(j = 2; j > 0 && i < inputsize; --j){
//			printf("%s %d \n","i in for loop:",i);
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
		while(i < inputsize && !(input[i] & '\x40')){
			++i;
		}

			
		if(i < inputsize){
			temp = input[i];
			++i;
			temp = temp & '\x06';
			temp = temp >> 1;
			byt = byt|temp;
		}
//	printf("\n%d\n", k);
	output[k] = byt;
//	printf("%s %d \n","i is:",i);
	}
	return k+1;
}

int main(int argc, char *argv[]){
	if(argc == 3){

		// Define Input Memory Buffer
	char *input = NULL;
	FILE *ifp = fopen(argv[1],"r");
	long inputsize = 0;
	if(ifp != NULL){
	 /* Go to the end of the file */
	if(fseek(ifp, 0L, SEEK_END)== 0){
		/*Get the size of the file. */

		inputsize = ftell(ifp);
		if (inputsize == -1) {
			fputs("Error finding size of file", stderr);
		 }
	
		/*Allocate our buffer of that size +1 for null termination. */
		input = malloc (sizeof(char) * (inputsize+1));
		

		/* Return to start of file*/
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
		}

		/*Read the entire file into memory*/
		size_t newLen = fread(input, sizeof(char), inputsize, ifp);
		if(newLen == 0){
			fputs("Error reading file", stderr);
		} else {
			input[newLen++] = '\0'; // Null termination character
			}
		}			
		fclose(ifp);
	}else{
		printf("%s\n", "the input file given does not exist");
		return 1;
	}

		// Define Output Buffer;
	char * output = malloc(sizeof(char)* (inputsize+1));
	int outputsize = 0;
		// Read in all data from argv[1] and store in input memory buffer


//	int k;
//	for(k=0; k<inputsize; ++k){
//	printf("%s %c \n","Input[k]:", input[k]);
//	}
		// In a for loop, run the twoBit Funtion using a timer function, reading from input memory buffer and writting to output 
		// memory buffer. Store the run times in an array
	
	clock_t start, diff;
	start = clock();;
	outputsize=twoBit(input,output,inputsize);
	diff = clock() - start;
	printf("\n%d\n",diff);
		// Record fastest times from the 100 runs
		// Write from output memory buffer to an output file
		// Check for correctness
		//
		//
		//

	int i;
	FILE *ofp = fopen(argv[2],"w");
	for(i=0; i<outputsize; ++i){
		fprintf(ofp,"%c",output[i]);
	}
		fclose(ofp);
		free(input); // Frees memory used for input buffer
		free(output);
		return 0;

	

        }else{
		 return 1;
	}
 }
