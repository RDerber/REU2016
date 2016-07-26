/*
 * 2BitToFasta.c
 *
 * EOF issue is resolved 
 *
 * 
 */


#include <stdio.h>
#include <stdlib.h>

int bitSeparator(char byt, FILE * ofp){
	int i;
	for(i = 3; i>0; --i){
		char temp = byt & ('\x03' << (2*i));//get nucleotide from byte
		temp =  (temp >> (i*2)-1)&'\x06';//shift to position in ascii char and make sure no ones were carried
 		if(!(temp^'\x04')){//if temp is a T, not A,G,C
		temp = 'T';
		}else{
			temp = temp | '\x41';   //convert to ascii char
		}
		fprintf(ofp, "%c", temp);
   	}
	byt = byt & '\x03';//get last nucleotide
	byt = byt << 1;//shift to position in ascii char
	if(!(byt^'\x04')){//if temp is a T, not A,G,C
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
	char flinec;
	while((flinec = getc(ifp)) != '\n'){
		fprintf(ofp, "%c", flinec);
	}	
	fprintf(ofp, "%c", '\n');
	int byt;
	while((byt = getc(ifp)) != EOF){
		bitSeparator(byt, ofp);
	}
	
	fclose(ofp);
	fclose(ifp);
//	free(lineptr);
	return 0;
}

int main(int argc, char *argv[]){
	if(argc == 3){
		twoBit(argv[1],argv[2]);
		return 0;
        }else{
		 return 1;
	}
 }
