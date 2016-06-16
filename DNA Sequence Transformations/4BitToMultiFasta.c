/*
 * 4BitToMultiFasta.c
 *
 *
 *
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>

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
                lobyt = lobyt | '\x40'; //convert to capital character
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
	while((head = getc(hfp)) != '\x00')
		fprintf(ofp,"%c", head);
	fprintf(ofp,"\n");
	while((byt=getc(ifp))!= EOF){
	
		if(byt=='<'){
			fprintf(ofp, "\n");
			while((head = getc(hfp)) != '\x00')
				fprintf(ofp,"%c",head);
			fprintf(ofp,"\n");
			byt = getc(ifp);
		}

	bitSeparator(byt, ofp);
	}
	fclose(ofp);
	fclose(ifp);
	return 0;
}

int main(int argc, char *argv[]){
	if(argc == 4){
		fourBit(argv[1],argv[2], argv[3]);
		return 0;
        }else{
		 return 1;
	}
 }
