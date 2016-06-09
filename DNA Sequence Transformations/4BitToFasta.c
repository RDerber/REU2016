/*
 * 4BitToFasta.c
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

	char flinec;
	while((flinec = getc(ifp)) != '\n'){
		fprintf(ofp, "%c", flinec);
	}	
	fprintf(ofp, "%c", '\n');
	int byt;
	int eof = 0;//not end of file
	while((byt=getc(ifp))!= EOF){
	
	bitSeparator(byt, ofp);

	}
	fclose(ofp);
	fclose(ifp);

	return 0;
}

int main(int argc, char *argv[]){
	if(argc == 3){
		fourBit(argv[1],argv[2]);
		return 0;
        }else{
		 return 1;
	}
 }
