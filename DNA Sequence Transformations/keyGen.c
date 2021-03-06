/*
 * keyGen.c
 *
 * Generates a random input/output key of ascii characters in the range 33 (!) to 126 (~). 
 *
 * Parameters:
 * 	[key size] [output folder path] [random number seed]
 *
 * 	The output file will be placed in the given folder with the automatically generated name: "key(keySize/2).txt"
 *		Ex. If keySize is 20, then the key generated will be named key10.txt
 * 		This custom was adopted to suite the our need to make many random keys quickly for testing and timing functions 
 *
 *	The random number seed is an random number that will be used to seed the random number generator used to create the keys
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int writeKey(char *fileBuf, int *xvals, int *yvals, int numVals){
	int i,k=0,f=0;
	int len = 20;
	char numBuf[len];
	for(i=0;i<numVals;++i){
//		if(xvals[i] < 32){
//			printf("x");
//			memset(numBuf,0,len*sizeof(char));
//			snprintf(numBuf,len,"%d",xvals[i]);
//			printf("%s\n",numBuf);
//			k=0;
//			while(numBuf[k] != '\x00') fileBuf[f++] = numBuf[k++];
//		}else{
			fileBuf[f++] = xvals[i];
//		}
		fileBuf[f++] = '>';
//		if(yvals[i] < 32){
//			printf("y");
//			snprintf(numBuf,len,"%d",yvals[i]);
//			printf("%s\n",numBuf);
//			k=0;
//			while(numBuf[k] != '\x00') fileBuf[f++] = numBuf[k++];
//		}else{
			fileBuf[f++] = yvals[i];
//		}
		fileBuf[f++] = '\n';
	}
	fileBuf[f] = '\x00'; 
	return f;
}

int main(int argc,char **argv){	//[key size] [output folder path] [random number seed]
	
	if(argc != 4){
		printf("Invalid Arguments. Need: [key size] [output folder path] [random number seed]");
		return -1;
	}
	
	int keySize = atoi(argv[1]);
	int i,j,k;
	srand(atoi(argv[3]));
	int xvals[keySize];
	int yvals[keySize];
	for(j=0;j<keySize;++j){
		xvals[j] = (rand()%94)+33;
		if(xvals[j] == 62)--j;
		for(k=0;k<j;++k){	
			if(xvals[j] == xvals[k]){
				--j;
				break;
			}
		}
	}
	for(j=0;j<keySize;++j){
		yvals[j] = (rand()%94)+33;
		if(yvals[j] == 62)--j;
		for(k=0;k<j;++k){
			if(yvals[j] == yvals[k] || yvals[j] == 62){
				--j;
				break;
			}
		}
	}
	char *output = malloc(keySize*4* sizeof(char));
	int oFSize = writeKey(output, xvals, yvals, keySize);
//	printf(output);
//	printf("%s %d","oFSize:",oFSize)
	char outName[100];
	snprintf(outName,100,"%s%s%d%s",argv[2],"key",keySize/2,".txt");
	FILE *ofp = fopen(outName, "w");
	fwrite(output, sizeof(char), oFSize, ofp);
	free(output);
	fclose(ofp);
//	for(i=0;i<keySize;++i){
//		printf("%s%d %s%d\n", "xvals[i]:", xvals[i], "yvals[i]", yvals[i]);
//		fflush(stdout);
//	}
	
	return 0;	
}
