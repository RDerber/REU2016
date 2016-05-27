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
	while(!((byt = getc(ifp))&'\x80')){//get first char and shift right
//	if(byt != '\n' && byt != '\r'){ // mask with x40
		if(byt & '\x40'){ // x40 is 0100 0000, performs above test without comparisons
			byt = byt & '\x06';
			byt = byt >> 1;
		}
		int i;
		for(i = 1; i<4;++i){//get next three nucleotides shifting
					//left accordingly
			char temp = getc(ifp);
//			if(temp != EOF && temp != '\n' && temp != '\r'){
			if(temp != EOF){
			if(temp&'\x40' && !(temp&'\x80')){//performs above test w/o comparisons
				temp = temp & '\x06';
				temp = temp << (i*2)-1;
				byt = byt|temp;
			}
		}
		}
		
		fprintf(ofp, "%c", byt);
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
