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
	char byt;
	int eof = 0;
	while(!eof){
		byt = getc(ifp);
		if(!(byt & '\x80')){
			int i;
			for(i = 3; i>0; --i){	//get next three nucleotides shifting
						//left accordingly
				char temp = byt & ('\x03' << (2*i));
				temp =  (temp >> (i*2)-1)&'\x06';
				if(!(temp^'\x04')){//if temp is a T, not A,G,C
					temp = 'T';
				}
				else{
					temp = temp | '\x41';
				}
				fprintf(ofp, "%c", temp);
			}
			byt = byt & 3;
			byt = byt << 1;
			if(!(byt^'\x04')){//if temp is a T, not A,G,C
				byt = 'T';
			}
			else{
				byt = byt | '\x41';
			}
			fprintf(ofp, "%c", byt);
			fprintf(ofp, "%c", '\n');
		}else{
			int i;
                        char checkeof;
                        char eofarr[30];
                        while(((checkeof=getc(ifp))&'\x80') && i < 30){
                                checkeof = getc(ifp);
                                eofarr[i]=checkeof;
                                ++i;
                        }
                        if(i!=30){
                                for(i = i-1; i=0;--i){
                                        ungetc(eofarr[i],ifp);
                                }
                        }else{
                                eof = 1;
                        }
		}
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
