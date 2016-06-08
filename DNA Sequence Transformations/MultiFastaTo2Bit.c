/*
 * MultiFastaTo2Bit.c
 *
 *Inputs:
 * [FASTA input file] [Name of 2-bit output file]
 *
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
	char header;
	while((byt = getc(ifp))!= EOF){//Get first character and check for EOF
//	if(byt != '\n' && byt != '\r'){ // mask with x40
		int endseq = 0;
		while(!(byt&'\x40')){ // Check for \n, \r characters
			if(byt=='>'){ // Check for > Character
				fprintf(ofp,"%s","New Sequence Starts Below"); 
													//FASTA
				fprintf(ofp, "%c%c",'\n', '>');
				while((header = getc(ifp)) != '\n' && header != EOF){//
					fprintf(ofp, "%c", header );
			}

			fprintf(ofp, "%c", '\n');
			fflush(ofp);
		//	byt = header; 
		}
	//	else{
			byt = getc(ifp);
	//	}	
		}
		if(byt== EOF){
			break;
		}
		byt = byt & '\x06';
		byt = byt << 5;
		int temp;
		int i;
		for(i = 2; i>0;--i){//get next two nucleotides shifting
					//left accordingl
			temp = getc(ifp);
//			if(temp != '\n' && temp != '\r'){
			if(temp & '\x40'){//performs above test w/o comparisons
				if(temp == EOF){
					fprintf(ofp,"%c",byt);
					break;
				}
				temp = temp & '\x06';
				temp = temp << (i*2)-1;
				byt = byt|temp;
			}else{
				if(temp == '>'){
					fprintf(ofp,"%c",byt);
					fprintf(ofp,"%s","New Sequence Starts Below"); 
//					fprintf(ofp,"%c %c %c %c %c %c %c %c",' ',' ',' ',' ',' ',' ',' ',' '); // 8 spaces used to transfer
														// back to FASTA
                                        fprintf(ofp,"%c%c", '\n', '>');
					endseq = 1;
					while((temp = getc(ifp)) != '\n'){
						fprintf(ofp,"%c",temp);
					}
					fprintf(ofp,"%c",'\n');
					break;
				}
				++i;
			}
		}
		if(!endseq){
			while(!((temp=getc(ifp)) & '\x40')){//while temp is null, \n or \r
                                                                        //extract characters
                                                                        //
				if(temp == '>'){
					endseq = 1;
					fprintf(ofp,"%c",byt);
					fprintf(ofp,"%s","New Sequence Starts Below");
					fprintf(ofp, "%c%c",'\n','>');
					while((header = getc(ifp)) != '\n' && header != EOF){
                        	        	fprintf(ofp, "%c", header );
                        		}	

                        	fprintf(ofp, "%c", '\n');
                  		fflush(ofp);
				break;
               			}	
			}
		}
		if(!endseq && temp != EOF){
			temp = temp&'\x06';
			temp = temp >> 1;
			byt = byt|temp;
			fprintf(ofp, "%c", byt);
		}
		fflush(ofp);
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
