/*
 * FastaTo2Bit.c
 *
 * Functional for regular FASTA
 *
 *
 *
 */



#include <stdio.h>
#include <stdlib.h>

int twoBit(const char * filename,const char * outFileName){
	FILE * ifp;
	FILE * ofp;
	ifp = fopen(filename,"r");
//	char * lineptr = NULL;
//	size_t * len = 0;
//	getline(&lineptr,len, ifp);
//	char newFileName[100];
//	printf("Give a name to the file this program will create and write to:" );
//	gets(newFileName);
	ofp = fopen(outFileName, "w");
	char flinec;
	while((flinec = getc(ifp)) != '\n'){
		fprintf(ofp, "%c", flinec);
	}
	fprintf(ofp, "%c", '\n');
	fflush(ofp);
	char byt;
	while((byt = getc(ifp))!= EOF){//get first char and shift right
//	if(byt != '\n' && byt != '\r'){ // mask with x40
		while(!(byt&'\x40')){
			byt = getc(ifp);
		}
		if(byt== EOF) break;
		byt = byt & '\x06';
		byt = byt << 5;
		char temp;
		int i;
		for(i = 2; i>0;--i){//get next two nucleotides shifting
					//left accordingly
			temp = getc(ifp);
//			if(temp != '\n' && temp != '\r'){
			if(temp&'\x40'){//performs above test w/o comparisons
				temp = temp & '\x06';
				temp = temp << (i*2)-1;
				byt = byt|temp;
			}else{
				++i;
			}
		}
		while(!((temp=getc(ifp)) & '\x40')){}	//while temp is null, \n or \r
									//extract characters
		temp = temp&'\x06';
		temp = temp >> 1;
		byt = byt|temp;
		fprintf(ofp, "%c", byt);
		fflush(ofp);
	}
	fclose(ofp);
	fclose(ifp);
//	free(lineptr);
	return 0;
}

int main(int argc, char *argv[]){
	if(argc == 3){

		// Define Input Memory Buffer
	char *input = NULL;
	File *fp = fopen(argv[1],"r");

	if(fp != NULL){
	 /* Go to the end of the file */
	if(fseek(fp, 0L, SEEK_END)== 0){
		/*Get the size of the file. */

		long bufsize = ftell(fp);
		if (bufsize == -1) {
			fputs("Error finding size of file", stderr);
		 }
	
		/*Allocate our buffer of that size +1 for null termination. */
		input = malloc (sizeof(char) * (bufsize+1));
		

		/* Return to start of file*/
		if(fseek(fp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
		}

		/*Read the entire file into memory*/
		size_t newLen = fread(input, sizeof(char), bufsize, fp);
		if(newLen == 0){
			fputs("Error reading file", stderr);
		} else {
			input[newLen++] = '\0'; // Null termination character
			}
		}			
		fclose(fp);
	}

		// Define Output Buffer




		 // do this at the end: free(input);


		// Read in all data from argv[1] and store in input memory buffer


		// In a for loop, run the twoBit Funtion using a timer function, reading from input memory buffer and writting to output 
		// memory buffer. Store the run times in an array
	
		
		twoBit(argv[1],argv[2]);

		// Record fastest times from the 100 runs
		// Write from output memory buffer to an output file
		// Check for correctness
		//
		return 0;



        }else{
		 return 1;
	}
 }
