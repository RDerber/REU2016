/*
 * 2BitToMultiFasta.c
 *
 * Converts 2Bit compressed Multi-FASTA data back into a Multi-FASTA format file
 *
 * Parameters: 
 * [Input 2Bit File Name] [Output FASTA File Name] [Input Header File] [Input Positions File]
 * 
 * Header file: (null delimited header lines)
 * Position file: (positions, in bytes, of each sequence in the 2bit file, delimited with -1)
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
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/****************************************************************
 * bitSeparator:
 * Separates each byte into 2 bit sequences 
 * and converts each sequence to its ascii character
 ***************************************************************/
int bitSeparator(char byt, FILE * ofp){

	char byt1;
	char byt2;
	char byt3;
	char byt4;
	byt1 = byt>>5;	//get first nucleotide
	byt1 &= '\x06';
	byt1 |= '\x41';	//convert to capital character
	if(!(byt1 ^ '\x45')){
		byt1 = 'T';
	}
	byt2 = byt>>3;
	byt2 &= '\x06'; 	//get last nucleotide
	byt2 |= '\x41'; 	//convert to capital character
	if(!(byt2 ^ '\x45')){
		byt2 = 'T';
	}
	byt3 = byt>>1;
	byt3 &= '\x06'; 	//get last nucleotide
	byt3 |= '\x41'; 	//convert to capital character
	if(!(byt3 ^ '\x45')){
		byt3 = 'T';
	}
	byt4 = byt<<1;
	byt4 &= '\x06'; 	//get last nucleotide
	byt4 |= '\x41'; 	//convert to capital character
	if(!(byt4 ^ '\x45')){
		byt4 = 'T';
	}
	fprintf(ofp, "%c", byt1);
	fprintf(ofp, "%c", byt2);
	fprintf(ofp, "%c", byt3);
	fprintf(ofp, "%c", byt4);
	return 0;
                }
	



int twoBit(const char * filename,const char * outFileName, const char * headerFileName, const char * positionFileName){ 
	FILE * ifp;
	FILE * ofp;
	FILE * hfp;
	FILE * pfp;
	if((ifp = fopen(filename,"r")) == NULL){
		printf("Error opening input file");
		return -1; 
		}
	if((ofp = fopen(outFileName, "w"))==NULL){
		printf("Error creating output file");
		return -1; 
		}
	if((hfp = fopen(headerFileName, "r")) == NULL){
		printf("Error opening header file");
		return -1; 
		}
	if((pfp = fopen(positionFileName, "r")) == NULL){
		printf("Error opening position file");
		return -1; 
		}
	int byt;
	int head;
	int position;
	int i;
	long startPoint = 3;
	int end = 0;
	int seqPos = 0;
	while(!end){
		// Read in starting position of the sequence //
		int posLength = 0;
		fseek(pfp, startPoint, SEEK_SET);
		while((position = getc(pfp))!= '-'){
			if(position == EOF){
				end = 1;
				break;
			}
			posLength++;
		}
		char *posNumber = malloc(sizeof(char)*(posLength+1));
		fseek(pfp, startPoint, SEEK_SET);
		startPoint += (posLength+2);

		for(i=0; i<posLength; ++i){
			char num = getc(pfp);
			posNumber[i] = num; 
		}
		posNumber[posLength]='\x00';
		fseek(pfp, startPoint, SEEK_SET);
		i = seqPos;
		if(!end){
			seqPos = atoi(posNumber);
		}else seqPos = INT_MAX;
		free(posNumber);
		
		// Print Header Line
		fprintf(ofp,"%c",'>');
		while((head = getc(hfp)) != '\x00' && head != EOF)
			fprintf(ofp,"%c",head);
		fprintf(ofp,"\n");
		
		//Print Sequence	
		while(i < seqPos){
			if((byt=getc(ifp))!= EOF){
			bitSeparator(byt, ofp);
			++i;
			}else{
				end = 1;
				break;
			}
		}
		fprintf(ofp, "\n");
	}
	
	fclose(ofp);
	fclose(ifp);
	return 0;
}

int main(int argc, char *argv[]){	//[Input 2Bit File Name] [Output FASTA File Name] [Input Header File] [Input Positions File]
	if(argc == 5){
		twoBit(argv[1],argv[2], argv[3],argv[4]);
		return 0;
	}else{
		printf("incorrect arguments for 2BitToMultiFasta\n");
		return -1;
	}
 }
