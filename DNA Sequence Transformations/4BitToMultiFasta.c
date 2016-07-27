/*
 * 4BitToMultiFasta.c
 *
 *Converts 4Bit compressed Multi-FASTA data back into a Multi-FASTA format file
 *
 * Parameters: 
 * [Input 4Bit File Name] [Output FASTA File Name] [Input Header File] [Input Positions File]
 *
 * Header file: (null delimited header lines)
 * Position file: (positions, in bytes, of each sequence in the 2bit file, delimited with -1)
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
	upbyt = byt >> 4; 	//get first nucleotide
	upbyt = upbyt & '\x0f';
	upbyt = upbyt | '\x40';	//convert to capital character
	if(!(upbyt ^ '\x44')){
		upbyt = 'T';
	}
	lobyt = byt & '\x0f'; 	//get last nucleotide
	lobyt = lobyt | '\x40'; 	//convert to capital character
	if(!(lobyt ^ '\x44')){
		lobyt = 'T';
	}
	fprintf(ofp, "%c", upbyt);
	fprintf(ofp, "%c", lobyt);
	return 0;
}
	

int fourBit(const char * filename,const char * outFileName, const char * headerFileName){
	FILE * ifp;
	FILE * ofp;
	FILE * hfp;
	ifp = fopen(filename,"r");
	ofp = fopen(outFileName, "w");
	hfp = fopen(headerFileName, "r");
	int byt = getc(ifp);
	int head;
	
	// Print out first header line //
	while((head = getc(hfp)) != '\x00')
		fprintf(ofp,"%c", head);
	fprintf(ofp,"\n");	
	while((byt=getc(ifp))!= EOF){
	
		// Print out other header lines //
		if(byt=='<'){
			fprintf(ofp, "\n");
			while((head = getc(hfp)) != '\x00')
				fprintf(ofp,"%c",head);
			fprintf(ofp,"\n");
			byt = getc(ifp);
		}
	
	// Convert 4Bit sequences to ASCII FASTA characters //
		bitSeparator(byt, ofp);
	}
	fclose(ofp);
	fclose(ifp);
	return 0;
}

int main(int argc, char *argv[]){ //[Input 4Bit File Name] [Output FASTA File Name] [Input Header File] [Input Positions File]
	if(argc == 4){
		fourBit(argv[1],argv[2], argv[3]);
		return 0;
	}else{
		printf("incorrect arguments for 4BitToMultiFasta\n");
		return -1;
	}
 }
