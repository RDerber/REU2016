#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define IDX3_OFFSET_NUM         7
#define IDX3_OFFSET_ARRAYSTART 19
#define IDX3_LENGTH            28
#define IDX3_ARRAYSIZE        784

#define FILEPATH "t10k-images.idx3-ubyte"

size_t load_idx3ubyte_to_buffer(char *filepath, unsigned char **buffer) {
	
	FILE *file;
	long fileLength;

	file = fopen(filepath, "rb");
	if(file == NULL){
		printf("Cannot open file \n");
	} 
	fseek(file, 0, SEEK_END);
	fileLength = ftell(file);
	rewind(file);

	printf("%d\n", fileLength);

	*buffer = (unsigned char *)malloc(sizeof(unsigned char) * fileLength);
	fread(*buffer, fileLength, 1, file);
	fclose(file);
	printf("File successfully read \n");

	return fileLength;
}

void set_buffer_to_pixel_array(unsigned char *inputBuffer, 
			unsigned char **arrayBuffer, int array){
	*arrayBuffer = (unsigned char *)malloc(sizeof(unsigned char) * IDX3_ARRAYSIZE);

	array = 0; // change

	int arrayStart = ((IDX3_ARRAYSIZE) * array) + IDX3_OFFSET_ARRAYSTART;

	*arrayBuffer += *(inputBuffer + arrayStart);
}

int main(int argc, char **argv){
	int numImages, i;
	unsigned char *inputBuffer;
	unsigned char *arrayBuffer;
	size_t fileLength;

	fileLength = load_idx3ubyte_to_buffer(FILEPATH, &inputBuffer);

	printf("%d\n",fileLength );

	set_buffer_to_pixel_array(inputBuffer, &arrayBuffer, 0);

	numImages = *(inputBuffer + IDX3_OFFSET_NUM);

	printf("%d\n", numImages);

	for(i = 0; i < IDX3_ARRAYSIZE; i++){
	  printf("%c\n", arrayBuffer[i]);	
	}




	return 0;
	
}

