/*
 * 4BitToFasta.c
 * 
 * Converts 4Bit compressed FASTA data back into FASTA format files
 *
 * Parameters: 
 * [Input 4Bit File Name] [Output FASTA File Name] 
 *
 * Each four bit sequence will be converted to a nucleotide base:
 *		A = 0001
 *		C = 0011
 *		G = 0111
 *		T = 0100
 *		N = 1110
 *
 * The bases are then stored as ASCII characters, retaining the left-to-right order in which they were read
 *  Ex: 0001 1110 will convert to AN
 *
 * If the number of bases is not a multiple of 2, the last byte in the translation will end with (num bases % 2) pairs of zeros
 * 	These pairs of zeros will then translate back into FASTA as an extra '@' appended on the end of the sequence
 * 
 */


#include <stdio.h>
#include <stdlib.h>

/********************************************
 *bitSeparator:
 * separates bytes into two 4bit sequences 
 * and converts them back to ASCII base characters
 *************************************************/
int bitSeparator(char byt, FILE * ofp){
		char upbyt;
		char lobyt;
                
                upbyt = byt >> 4; //get first nucleotide
		upbyt = upbyt & '\x0f';
		upbyt = upbyt | '\x40';//convert to capital character
		if(!(upbyt ^ '\x44')){
			upbyt = 'T';
		}
                lobyt = byt & '\x0f'; //get last nucleotide
		lobyt = lobyt | '\x40';	//convert to capital character
		if(!(lobyt ^ '\x44')){
			 lobyt = 'T';
		}
                fprintf(ofp, "%c", upbyt);
		fprintf(ofp, "%c", lobyt);
		return 0;	
		}



int fourBit(const char * filename,const char * outFileName){
	FILE * ifp;
	FILE * ofp;
	ifp = fopen(filename,"r");
	ofp = fopen(outFileName, "w");

// Copy header (first) line from 4Bit file //
	char flinec;
	while((flinec = getc(ifp)) != '\n'){
		fprintf(ofp, "%c", flinec);
	}	
	fprintf(ofp, "%c", '\n');
	int byt;

// Convert the 4Bit sequences back into ASCII FASTA format //
	while((byt=getc(ifp))!= EOF){
	
	bitSeparator(byt, ofp);

	}
	fclose(ofp);
	fclose(ifp);

	return 0;
}

int main(int argc, char *argv[]){ //[Input 4Bit File Name] [Output FASTA File Name] 
	if(argc == 3){
		fourBit(argv[1],argv[2]);
		return 0;
	}else{
     printf("incorrect arguments for 4BitToFasta\n");
		 return -1;
	}
 }
