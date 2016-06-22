#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define PEN_UP "PEN_UP"
#define PEN_DOWN "PEN_DOWN"
#define INCLUDE "INCLUDE"
#define LEXICON "LEXICON"
#define COMMENT "COMMENT"
#define SEGMENT "SEGMENT"
#define HIERARCHY "HIERARCHY"

#define FILE_PATH "pendigits-orig.tes"

#define VECTOR_DIMMENSION 500
#define IMAGE_DIMMENSION 28

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

void drawSquare(unsigned char pixelArray[IMAGE_DIMMENSION][IMAGE_DIMMENSION], 
		double x, double y, double brushRadius){
	//take coordinate and exapnd/decrement in brush size dimmension
	//if any pixel exceeds the bounds, dont do anything
	// if(x < IMAGE_DIMMENSION || y < IMAGE_DIMMENSION || x >= 0 || y >= 0) {}
	int i, j;
	for (i = floor(x - brushRadius); i <= x + brushRadius; ++i) {
		for (j = floor(y - brushRadius); j <= y + brushRadius; ++j) {
			if (i >= IMAGE_DIMMENSION || j >= IMAGE_DIMMENSION || i < 0 || y < 0 || 
				pixelArray[i][j] == 255) {
				continue;
			} 
			else {
				double xDiff = (i < x) ? (i + 1) - (x - brushRadius) : 
						(x + brushRadius) - i;
				double yDiff = (j < y) ? (j + 1) - (y - brushRadius) : 
						(y + brushRadius) - j;
				if(i == 8 && j == 2){
					printf("%f %f\n", xDiff, yDiff);
				}
				if (xDiff < 1 && yDiff < 1) {
					pixelArray[i][j] = (unsigned char)((yDiff * xDiff) * 255);
					// pixelArray[i][j] = (xDiff < 1) ? 
					// 	(unsigned char)(xDiff * 256) : (unsigned char)(yDiff * 256);
				} else if (xDiff < 1 || yDiff < 1) { 
					// pixelArray[i][j] = (unsigned char)((yDiff * xDiff) * 256);
					pixelArray[i][j] = (xDiff < 1) ? 
						(unsigned char)(xDiff * 256) : (unsigned char)(yDiff * 255);
				} else {
					printf("\n%s", "reached");
					printf(" %d %d ", i, j);
					pixelArray[i][j] = (unsigned char)255;
				}
			}
		}
	}	
}

double calculate_b(int startX, int startY, double slope) {
	return startY - slope * startX;
}

double calculate_other_coordinate(char XY, int coordinate, double slope, 
		double b) {
	if(XY == 'x'){
		return (coordinate - b) / slope;
	} else {
		return coordinate * slope + b;
	}
}

void draw_line(unsigned char pixelArray[IMAGE_DIMMENSION][IMAGE_DIMMENSION], 
		double brushRadius, int startX, int startY, int endX, int endY) {
	double slope = (double)(endY - startY) / (endX - startX);
	double b = calculate_b(startX, startY, slope);
	//if abs(slope) is less than 1, do operations on x, more than 1, y
	//if start of point is less that end, increment, otherwise decrement
	int i;
	if (abs(slope) < 1) {
		if (startX > endX) {
			for (i = startX; i >= endX; --i) {
				double y = calculate_other_coordinate('y', i, slope, b);
				drawSquare(pixelArray, (double)i, y, brushRadius);
			}
		} else {
			for (i = startX; i <= endX; ++i) {
				double y = calculate_other_coordinate('y', i, slope, b);
				drawSquare(pixelArray, (double)i, y, brushRadius);
			}
		}
	} else {
		if (startY > endY) {
			for (i = startY; i >= endY; --i) {
				double x = calculate_other_coordinate('x', i, slope, b);
				drawSquare(pixelArray, x, (double)i, brushRadius);
			}
		} else {
			for (i = startY; i <= endY; ++i) {
				double x = calculate_other_coordinate('x', i, slope, b);
				drawSquare(pixelArray, x, (double)i, brushRadius);
			}
		}	
	}
}


void draw_pixel_array(int *coordinates, size_t numCoordinates){
	unsigned char pixelArray[IMAGE_DIMMENSION][IMAGE_DIMMENSION] = {0};
	draw_line(pixelArray, 1, 4, 7, 23, 15);
	int i, j;
	for (i = 0; i < IMAGE_DIMMENSION; ++i) {
		for (j = 0; j < IMAGE_DIMMENSION; ++j) {
			printf("%d ", (int)pixelArray[i][j]);
		}
		printf("\n");
	}
}

size_t load_coordinates(char *token){
	size_t arraySize = 0;
	int *coordinates = malloc(10000 * sizeof(int));

	char *coordinate = strtok(token, " ");
	while (coordinate != NULL) {
		if (atoi(coordinate) != 0) {
			coordinates[arraySize++] = atoi(coordinate);  
		}
		coordinate = strtok(NULL, " ");
	}

	draw_pixel_array(coordinates, arraySize);

	// int i;
	// for (i = 0; i < arraySize; ++i){
	// 	printf("%d\n", coordinates[i]);
	// }

	return arraySize;
}
	
int main(){
	char *data;
	size_t filesize = load_upen_to_buffer(FILE_PATH, &data);
	
	char *token = malloc(sizeof(char) * 1000);
	token = strtok(data, ".");

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
		} else {
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
