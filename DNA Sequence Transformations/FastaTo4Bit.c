/*
 * FastaTo4Bit.c
 *
 * Functional for regular FASTA
 *  Needs to be modified
 *
 *
 */



#include <stdio.h>
#include <stdlib.h>

int fourBit(const char * filename,const char * outFileName){
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
	while((byt = getc(ifp))!= EOF){//get first char and shift right
//	if(byt != '\n' && byt != '\r'){ // mask with x40
		while(!(byt&'\x40')){
			byt = getc(ifp);
		}
		if(byt== EOF) break;
		byt = byt & '\x0f';
		byt = byt << 4;
		char temp;
	//	int i;
	//	for(i = 2; i>0;--i){//get next two nucleotides shifting
	//				//left accordingly
	//		temp = getc(ifp);
//	//		if(temp != '\n' && temp != '\r'){
	//		if(temp&'\x40'){//performs above test w/o comparisons
	//			temp = temp & '\x06';
	//			temp = temp << (i*2)-1;
	//			byt = byt|temp;
	//		}else{
	//			++i;
	//		}
	//	}
		while(!((temp=getc(ifp)) & '\x40')){}	//while temp is null, \n or \r
									//extract characters
		temp = temp&'\x0f';
		byt = byt|temp;
		fprintf(ofp, "%c", byt);
		fflush(ofp);
	}
	fclose(ofp);
	fclose(ifp);
//	free(lineptr);
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
