/*
 * jsonTitle.c
 *
 * Inserts a title to the json format file, then adds opening bracket and
 *	 indents the rest of the contents 4 spaces inwards
 *
 */

#include <stdio.h>
#include <stdlib.h>

int jsonTitle(char* filename, char * title, char* titleType){
	
	FILE * fp;
	long inputSize = 0;
	char *input = NULL; 

	fp = fopen(filename,"r");
	
	if(fp != NULL){
	
		if(fseek(fp,0L,SEEK_END) == 0){ // Move to the end of the file
						// (and is successful)
			inputSize = ftell(fp);
			
			if(inputSize == -1){
				fputs("Error finding size of file", stderr);
			}

			input = malloc((inputSize+1)* sizeof(char)); // creates array to store file contents 
	
			if(fseek(fp,0L,SEEK_SET)!=0){ // Return to start of file
				fputs("Error returning to start of file", stderr);
			}

			size_t file_length = fread(input, sizeof(char),inputSize, fp);
			if(file_length == 0){
				fputs("Error reading file", stderr);
			} else {
				input[file_length] = '\0'; // Null termination character
				}
		} else {
			printf("%s\n", "Error moving to the end of the file");

		}
		fclose(fp);
	}else{
		printf("%s\n", "The file you are trying to modify does not exist");
		return 1;
	}

	int i;
	int numNewLine = 0;
//	printf("%s %d \n","inputSize:",inputSize);
	for(i=0; i<inputSize; ++i){
		if(input[i] == '\n'){
		numNewLine++;	
		}
	}
//	printf("%s %d \n","num new line:",numNewLine);
	long outputSize = inputSize+(numNewLine*4 + 4);
//	printf("%s %d\n","output size:", outputSize);
	char* output = malloc(outputSize+1*sizeof(char));
	int k = 0;
	for(k=0; k<4;++k){
		output[k] = ' ';
	}
	for(i=0; i<inputSize;++i){
		if(input[i] == '\n' && input[i + 1] != EOF){
			output[k] = input[i];
			++k;
			++i;
			int j;
			for(j=0;j<4; ++j){
				output[k]= ' ';	
				++k;
			}
		}
		output[k] = input[i];
		++k;
	}

	free(input);
	
	FILE * ofp;
	if((ofp = fopen(filename,"w")) == NULL){ // Open the file again for writing, erasing its contents
		printf("Error creating output file");
		return -1; 
	}


	if(titleType == NULL){
		fprintf(ofp,"%s%s%s","\"",title,"\": {\n"); //Write out Title at the beginning of the file
		fwrite(output, 1, outputSize, ofp);
		fprintf(ofp,"%s","\n}");
	
	}else if(!strcmp(titleType,"-c")){

     		fprintf(ofp,"%s%s%s","\"",title,"\": {\n"); //Write out Title at the beginning of the file
		fwrite(output, 1, outputSize, ofp);
		fprintf(ofp,"%s","\n},");

	} else if(!strcmp(titleType,"-t")){
     		fprintf(ofp,"%s%s%s","\"",title,"\": {\n"); //Write out Title at the beginning of the file
		fwrite(output, 1, outputSize, ofp);
		fprintf(ofp,"%s","\n}");	

	} else if(!strcmp(titleType, "-f")){
		fprintf(ofp,"%s%s%s","{\n\"",title,"\":{\n");
		fwrite(output, 1, outputSize, ofp);
		fprintf(ofp,"%s","\n}");

	}


	fclose(ofp);

	free(output);

	return 0;

}


int main(int argc, char ** argv){
	if(argc==3){
		jsonTitle(argv[1],argv[2],NULL);
		return 0;
	}else if(argc ==4){
		jsonTitle(argv[1],argv[2],argv[3]);
		return 0;
	}else{
		printf("Invalid Arguments");
		return -1;
	}
	
}
