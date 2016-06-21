#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define PEN_UP "PEN_UP"
#define PEN_DOWN "PEN_DOWN"
#define INCLUDE "INCLUDE"
#define LEXICON "LEXICON"
#define COMMENT "COMMENT"
#define SEGMENT "SEGMENT"
#define HIERARCHY "HIERARCHY"

#define FILE_PATH "pendigits-orig.tes"


size_t load_upen_to_buffer(char *filepath, char **buffer){
	//open file
	FILE *file = fopen(filepath, "r");
	if(file == NULL){
		printf("Cannot open file \n");
	} 

	//find the length of the file
	fseek(file, 0, SEEK_END);
	size_t fileLength = ftell(file);
	rewind(file);

	//allocate the correct amount of memory  
	*buffer = (char *)malloc(sizeof(char) * fileLength + 1);
	
	//read the file into the buffer
	fread(*buffer, fileLength, 1, file);
	fclose(file);
	printf("File successfully read \n");

	return fileLength;
}

size_t load_coordinates(char *token){
	char delimiters[2] = {' ', '\r'};

	size_t arraySize = 0;
	int *coordinates = malloc(10000 * sizeof(int));

	printf("%d\n", 1);

	char *coordinate = strtok(token, " ");
	while (coordinate != NULL) {
		if (atoi(coordinate) != 0) {
			coordinates[arraySize++] = atoi(coordinate);  
		}

		coordinate = strtok(NULL, " ");
	}

	int i;
	for (i = 0; i < arraySize; ++i){
		printf("%d\n", coordinates[i]);
	}

	return arraySize;
}

int main(){
	char *data;
	size_t filesize = load_upen_to_buffer(FILE_PATH, &data);
	
	char *token = malloc(sizeof(char) * 1000);
	token = strtok(data, ".");


	printf("%d\n", 1);

	int firstTime = 1;
	int i;
	// while(token != NULL){
	for (i = 0; i < 9; ++i) {
		//search each token for PEN_DOWN
		printf("%d\n", strlen(token));
		if(strstr(token, PEN_DOWN) != NULL){
			printf("%d\n", strlen(token));
			size_t arraySize = load_coordinates(token);
			printf("%d\n", arraySize);
		// //if PEN_DOWN, parse token into array of values
		}
		printf("%d\n", data);
		if (firstTime == 1) {
			data += (strlen(token) + 2) * sizeof(char);
		} else{
			data += (strlen(token) + 1) * sizeof(char);
		}

		printf("%d\n", data);		
		
		//create new array
		//itterate through coordinates and call draw line between every two
		token = strtok(data, ".");
		printf("%s\n", token);

		if(firstTime == 1){
			firstTime = 0;
		}
	}


}
