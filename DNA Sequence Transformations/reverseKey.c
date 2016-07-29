/*
 * reverseKey.c
 *
 * Generates a key that is the inverse of the one passed in as the first argument to it - switches the x and y values***
 *	***Do not use on a key that maps multiple x values to the same y value, since the inverse will be undefined*** 
 *
 * Parameters:
 *	[input key path] [output key path]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include 

int readInputToBuffer(FILE * ifp, char ** input, long * inputsize){
 	// Go to the end of the file //
	if(fseek(ifp, 0L, SEEK_END)== 0){
		// Get the size of the file. //
		*inputsize = ftell(ifp);
		if (*inputsize == -1) {
			fputs("Error finding size of file", stderr);
		return -1;
		 }
		//Allocate our buffer of that size +1 for null termination. //
		*input = malloc (sizeof(char) * ((*inputsize)+1));
		
		// Return to start of file //
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
			return -1;
		}
		//Read the entire file into memory//
		size_t newLen = fread(*input, sizeof(char), *inputsize, ifp);
		if(newLen == 0){
			fputs("Error reading file", stderr);
			return -1;
		} else {
			// Null termination character at the end of the input buffer //
			(*input)[newLen++] = '\0'; 
		}
		return 0;
	} else return -1;
}

int keyIdentifier(char * fileBuf,int *input,int *output){
	char byt; 
	int inputCount = 0; 
	int outputCount = 0;
	int i = 0, j,f=0;

	while((byt=fileBuf[f++])!= '\x00'){
		char numBuf[20];
		if(byt != ' ' && byt != '\n' && byt != '\r'){
			if(byt != '>'){
				i=0;
				numBuf[i++] = byt;
				numBuf[i++] = '\x00';
				if(atoi(numBuf) == 0 && numBuf[0] != '0'){ // If it's a character, cast and put into input
					input[inputCount++] = numBuf[0]; 
				}
				else{
					input[inputCount++] = atoi(numBuf); //If it's a number, convert to int and put into input
				}
			}else{
				while((byt = fileBuf[f++]) == ' ' || byt == '\n' || byt == '\r'){}
				i=0;
				while(byt != '\n'){
					 numBuf[i++] = byt;
					 byt=fileBuf[f++];
				}
				numBuf[i++] = '\x00';
				
			//	output[outputCount++] = atoi(numBuf);
			//	continue; 
				if(atoi(numBuf) == 0 && numBuf[0] != '0'){ // If it's a character, cast and put into output
					output[outputCount++] = numBuf[0]; 
				}
				else{
					output[outputCount++] = atoi(numBuf); //If it's a number, convert to int and put into output
				}
				
			}
		}
	}
	return inputCount;
} 

int writeKey(char *fileBuf, int *xvals, int *yvals, int numVals){
	int i,k=0,f=0;
	int len = 20;
	char numBuf[len];
	for(i=0;i<numVals;++i){
		if(xvals[i] < 32){
			printf("x");
			memset(numBuf,0,len*sizeof(char));
			snprintf(numBuf,len,"%d",xvals[i]);
			printf("%s\n",numBuf);
			k=0;
			while(numBuf[k] != '\x00') fileBuf[f++] = numBuf[k++];
		}else{
			fileBuf[f++] = xvals[i];
		}
		fileBuf[f++] = '>';
		if(yvals[i] < 32){
			printf("y");
			snprintf(numBuf,len,"%d",yvals[i]);
			printf("%s\n",numBuf);
			k=0;
			while(numBuf[k] != '\x00') fileBuf[f++] = numBuf[k++];
		}else{
			fileBuf[f++] = yvals[i];
		}
		fileBuf[f++] = '\n';
	}
	return f;
}

int main(int argc,char **argv){	//[input key path] [output key path]
	FILE * ifp;
	long iFSize;
	int keySize;
	char *input;
	int *xvals;
	int *yvals;
	
	ifp = fopen(argv[1],"r");
	if(ifp != NULL){
		readInputToBuffer(ifp, &input, &iFSize);
	} else{ 
		printf("key file not accessible.");	
		return -1; 
	}
	fclose(ifp);
	xvals = malloc(sizeof(int)*iFSize/2);
	yvals = malloc(sizeof(int)*iFSize/2);
	keySize = keyIdentifier(input,xvals,yvals);
	free(input);
	char *output = malloc(iFSize * sizeof(char));
	int oFSize = writeKey(output, yvals, xvals, keySize);
	int i;

	free(xvals);
	free(yvals);
	
	FILE *ofp = fopen(argv[2], "w");
	fwrite(output, sizeof(char), oFSize, ofp);
	
	free(output);
	fclose(ofp);
}
