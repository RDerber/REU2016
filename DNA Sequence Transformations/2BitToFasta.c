/*
 * 2BitToFasta.c
 * 
 * Converts 2Bit compressed FASTA data back into a FASTA format file
 *
 * Parameters: 
 * [Input 2Bit File Name] [Output FASTA File Name] 
 * 
 * Each two bit sequence will be converted to a nucleotide base:
 *		A = 00
 *		C = 01
 *		G = 11
 *		T = 10
 *
 * The bases are then stored as ASCII characters, retaining the left-to-right order in which they were read
 *  Ex: 0001 1110 will convert to ACGT
 *	
 * If the number of bases is not a multiple of 4, the last byte in the translation will end with (num bases % 4) pairs of zeros
 * 	These pairs of zeros will then translate back into FASTA as extra A's appended on the end of the sequence 
 */


#include <stdio.h>
#include <stdlib.h>

/****************************************************************
 * bitSeparator:
 * Separates each byte into 2 bit sequences 
 * and converts each sequence to its ascii character
 ***************************************************************/
int bitSeparator(char byt, FILE * ofp){ 
	int i;
	for(i = 3; i>0; --i){
		char temp = byt & ('\x03' << (2*i));	//get nucleotide from byte
		temp =  (temp >> (i*2)-1)&'\x06';	//shift to position in ascii char and make sure no ones were carried
 		if(!(temp^'\x04')){	//if temp is a T, not A,G,C
		temp = 'T';
		}else{
			temp = temp | '\x41';   //convert to ascii char
		}
		fprintf(ofp, "%c", temp);
   	}
	byt = byt & '\x03';	//get last nucleotide
	byt = byt << 1;	//shift to position in ascii char
	if(!(byt^'\x04')){	//if temp is a T, not A,G,C
		byt = 'T';
	}else{
		byt = byt | '\x41';     //convert to ascii char
	}
	fprintf(ofp, "%c", byt);
//	fprintf(ofp, "%c", '\n');
	return 0;	
	}



int twoBit(const char * filename,const char * outFileName){
	FILE * ifp;
	FILE * ofp;
	ifp = fopen(filename,"r");
	ofp = fopen(outFileName, "w");
	
	/* Copies header line (first line) from 2Bit file */ 
	char flinec;
	while((flinec = getc(ifp)) != '\n'){
		fprintf(ofp, "%c", flinec);
	}	
	fprintf(ofp, "%c", '\n');
	
	/* Converts the remaining characters back into FASTA format */
	int byt;
	while((byt = getc(ifp)) != EOF){
		bitSeparator(byt, ofp);
	}
	
	fclose(ofp);
	fclose(ifp);
	return 0;
}

int main(int argc, char *argv[]){ // [Input 2Bit File Name] [Output FASTA File Name] 
	if(argc == 3){
		twoBit(argv[1],argv[2]);
		return 0;
	}else{
  	printf("incorrect arguments for 2BitToFasta\n");
		return -1;
	}
 }
