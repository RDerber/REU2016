#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_FILEPATH "optdigits-orig.tra"
#define OUTPUT_FILEPATH ""
#define LINE_OFFSET 3
#define MAX_LINE_SIZE 60


size_t load_input_buffer(char *filepath, char **buffer) {
	FILE *file = fopen(filepath, "r");
	if (file == NULL) {
		printf("Cannot open file \n");
	} 

	// find the length of the file
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	*buffer = (char *)malloc(sizeof(char) * fileSize + 1);
	
	fread(*buffer, fileSize, 1, file);
	fclose(file);
	printf("File successfully read \n");

	return fileSize;
}


int tokenize(char *buffer) {
    char *token;
    char *fileDelimiters = "\n";
    token = strtok(buffer, fileDelimiters);
    printf("%s", token);
}

void main() {
    // char *inputBuffer;
    // char *outputBuffer;
    
    // size_t inputSize = load_input_buffer(INPUT_FILEPATH, 
    //         &inputBuffer);
    
    // printf("reached\n");
            
    // tokenize(inputBuffer);
    
    // free(inputBuffer);
    
    printf("%d\n", atoi(" 27"));
}