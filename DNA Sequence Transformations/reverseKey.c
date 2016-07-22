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

int keyIdentifier(FILE * ifp,int *input,int *output){
	char byt; 
	int inputCount = 0; 
	int outputCount = 0;
	int i = 0, j;
	char numBuf[20];
	while((byt=getc(ifp))!= EOF){
	
		if(byt != ' ' && byt != '\n' && byt != '\r'){
			if(byt != '>'){
				input[inputCount++] = -byt;
			}else{
				while((byt = getc(ifp)) == ' ' || byt == '\n' || byt == '\r'){}
				i=0;
				while(byt != '\n'){
					 numBuf[i++] = byt;
					 byt=getc(ifp);
				}
				numBuf[i++] = '\x00';
				
			//	output[outputCount++] = atoi(numBuf);
			//	continue; 
				
				for(j=0; j < i-1; ++j){
					if(atoi(numBuf) == 0 && numBuf[j] != '0'){ // If it's a character, cast and put into output
						output[outputCount++] = (int)numBuf[j]; 
						break;
					}
					else{
						output[outputCount++] = atoi(numBuf); //If it's a number, convert to int and put into output
						break;
					}
				}
				
			}
		}
	}
	return inputCount;
} 

int main(int argc,char **argv){	//[input key path] [output key path]
	FILE * ifp;
	int keySize;
	char *input;
	char *xvals;
	char *yvals;
	
	ifp = fopen(argv[1],"r");
	if(ifp != NULL){
		keySize = getFileSize(kfp);
	} else{ 
		printf("key file not accessible.");	
		return -1; 
	}
	xvals = malloc(sizeof(char)*keySize);
	yvals = malloc(sizeof(char)*keySize);
}
