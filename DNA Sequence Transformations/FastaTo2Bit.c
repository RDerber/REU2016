/*
 * FastaTo2Bit.c
 *
 * Functional for regular FASTA
 *
 *  Issues: Prints \00 at beginning of 2Bit
 *
 * Once converted back: AAAA at beginning, extra T at end
 */



#include <stdio.h>
#include <stdlib.h>

int twoBit(char * input, char * output, long inputsize){
	size_t firstlnlen;
	for(firstlnlen = 0; input[firstlnlen] != '\n'; ++firstlnlen){
		output[firstlnlen] = input[firstlnlen];
	}
	output[firstlnlen]='\n';
	long i = firstlnlen + 1;
	long k = i;
//	printf("%s %d %s %d \n","firstlnlen:",firstlnlen,"inputsize:",inputsize);
	while(i < inputsize){
//	printf("%d \n",i);
//	printf("%s %c \n","Input[i]:",input[i]);
		while(!(input[i]&'\x40') && i < inputsize){
			++i;
		}
		char byt = input[i];
		byt = byt &'\x06';
		byt = byt << 5;
		char temp;
		unsigned j;
		for(j = 2; j > 0 && i < inputsize; --j){
			++i;
			if(input[i] & '\x40'){
				temp = input[i];
				temp = temp & '\x06';
				temp = temp << (j*2)-1;
				byt = byt|temp;
			} else {
				++j;
			}
		}
		while(i < inputsize && !(input[i] & '\x40')){
			++i;
		}
		if(i < inputsize){
			++i;
			temp = input[i];
			++k;
			temp = temp & '\x06';
			temp = temp >> 1;
			byt = byt|temp;
		}
		else{
			++k;

		}
	output[k] = byt;
	++i;
	}

	return k;
}

//	while((byt = getc(ifp))!= EOF){//get first char and shift right
//	if(byt != '\n' && byt != '\r'){ // mask with x40
//		while(!(byt&'\x40')){
//			byt = getc(ifp);
//		}
//		if(byt== EOF) break;
//		byt = byt & '\x06';
//		byt = byt << 5;
//		char temp;
//		int i;
//		for(i = 2; i>0;--i){//get next two nucleotides shifting
//					//left accordingly
//			temp = getc(ifp);
//			if(temp != '\n' && temp != '\r'){
//			if(temp&'\x40'){//performs above test w/o comparisons
//				temp = temp & '\x06';
//				temp = temp << (i*2)-1;
//				byt = byt|temp;
//			}else{
//				++i;
//			}
//		}
//		while(!((temp=getc(ifp)) & '\x40')){}	//while temp is null, \n or \r
//									//extract characters
//		temp = temp&'\x06';
//		temp = temp >> 1;
//		byt = byt|temp;
//		fprintf(ofp, "%c", byt);
//		fflush(ofp);
//	}
//	fclose(ofp);
//	fclose(ifp);
//	free(lineptr);
//	return 0;
//}

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
	}

		// Define Output Buffer;
	char output[inputsize];
	int outputsize = 0;
		// Read in all data from argv[1] and store in input memory buffer


		// In a for loop, run the twoBit Funtion using a timer function, reading from input memory buffer and writting to output 
		// memory buffer. Store the run times in an array
	
		
	outputsize=twoBit(input,output,inputsize);

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
		return 0;

	

        }else{
		 return 1;
	}
 }
