/*
 * MultiFastaTo4Bit.c
 *
 *Inputs:
 * [FASTA input file] [Name of 4-bit output file]
 *
 *
 *    
 *
 */





#include <stdio.h>
#include <stdlib.h>

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
	fflush(ofp);
	int byt;
	char header;
	while((byt = getc(ifp))!= EOF){//Get first character and check for EOF
		int endseq = 0;
		while(!(byt&'\x40')){ // Check for \n, \r characters
			if(byt=='>'){ // Check for > Character
				fprintf(ofp,"%c",'<'); 
													//FASTA
				fprintf(ofp, "%c%c",'\n', '>');
				while((header = getc(ifp)) != '\n'){//
					fprintf(ofp, "%c", header );
				}

				fprintf(ofp, "%c", '\n');
				fflush(ofp);
			}
				
			byt = getc(ifp);
		}

		if(byt == EOF){
			break;
		}
		byt = byt << 4;
		int temp;
		while(!((temp = getc(ifp)) & '\x40')){
			if(temp == '>'){
                		fprintf(ofp,"%c",byt);
                    		fprintf(ofp,"%c",'<'); 
                        	fprintf(ofp,"%c%c", '\n', '>');
                        	endseq = 1;
                        	while((temp = getc(ifp)) != '\n'){
                			fprintf(ofp,"%c",temp);
                    		}
                        fprintf(ofp,"%c",'\n');	
			break;		
			}
		}
		if(!endseq){
			temp = temp & '\x0f';
			byt = byt|temp;
			fprintf(ofp,"%c",byt);
		}
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
