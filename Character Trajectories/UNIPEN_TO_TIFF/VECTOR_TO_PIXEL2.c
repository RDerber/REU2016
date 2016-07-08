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
#define SQRT_2 1.4142


//TIFF HEADER CODES
#define HEADER_ENDIAN_LITTLE            0x4949
#define HEADER_ENDIAN_BIG               0x4D4D
#define HEADER_MAGIC_NUMBER             0x002A

//TIFF TAG ID CODES                           
#define TAG_WIDTH                       0x0100
#define TAG_HEIGHT                      0x0101	
#define TAG_BITS_PER_SAMPLE             0x0102
#define TAG_COMPRESSION                 0x0103
#define TAG_INTERPRETATION              0x0106
#define TAG_STRIP_OFFSETS               0x0111
#define TAG_ROWS_PER_STRIP              0x0116
#define TAG_STRIP_BYTE_COUNTS           0x0117
#define TAG_X_RESOLUTION                0x011a
#define TAG_Y_RESOLUTION                0x011b
#define TAG_RESOLUTION_UNIT             0x0153
#define TAG_ORIENTATION                 0x0112

#define NUM_TAGS                            12
#define TIFF_END                    0x00000000

size_t load_upen_to_buffer(char *filepath, char **buffer){
	//open file
	FILE *file = fopen(filepath, "r");
	if (file == NULL) {
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

void drawSquare(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double x, double y, double brushRadius){
	//take coordinate and exapnd/decrement in brush size dimmension
	//if any pixel exceeds the bounds, dont do anything
	// if(x < VECTOR_DIMMENSION || y < VECTOR_DIMMENSION || x >= 0 || y >= 0) {}
	int i, j;
	for (i = floor(x - brushRadius); i <= x + brushRadius; ++i) {
		for (j = floor(y - brushRadius); j <= y + brushRadius; ++j) {
			if (i >= VECTOR_DIMMENSION || j >= VECTOR_DIMMENSION || i < 0 || j < 0 || 
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

// void drawCircle(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
// 		double x, double y, double brushRadius){
	
// 	double prevY = brushRadius;
// 	double nextY;
// 	int i;
// 	for (i = floor(x); i <= x + SQRT_2 * brushRadius; ++i) {
		
// 		int base = 1;
// 		double height = sqrt(brushRadius * brushRadius)
// 	}
// 	// iterate through xs from the x to x + radius/sqrt(2)
// 	// prevdistance = r; 

// 	// find next distance: sqrt(rsquared - xsquared);
// 	// find diff between that and ceil that and prevdistance and ceil that
// 	// add and divide by two * 255 to find interpolated value
// 	// iterate downwards and fill until hits y
// 	// next distance = prev distance
// 	// reflect octant to all other octants
// }

int is_bounded_filled(int x, int y, 
		unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION]) {
	if (x >= VECTOR_DIMMENSION || y >= VECTOR_DIMMENSION || 
  		x < 0 || y < 0 || pixelArray[x][y] == 255) {
		return 0;
	} else {
		return 1; 
	}
}

void flood_fill(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		int x, int y){
	if (pixelArray[x][y] == 0 && is_bounded_filled(x, y, pixelArray)) {
		pixelArray[x][y] = 255;
		
		flood_fill(pixelArray, x - 1, y);
		flood_fill(pixelArray, x + 1, y);
		flood_fill(pixelArray, x, y - 1);
		flood_fill(pixelArray, x, y + 1);
	} else if (is_bounded_filled(x, y, pixelArray)) {
		pixelArray[x][y] = 255;
		return;
	} else {
		return;
	}
}

// void find_fill(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
// 		int x, int y, int seedFound) {
// 	if (seedFound == 1){
// 		return;
// 	} else if (pixelArray[x][y] == 0) {
// 		flood_fill(pixelArray, x, y);
// 		seedFound = 1;
// 		return;
// 	} else {
// 		find_fill(pixelArray, x - 1, y, seedFound);
// 		find_fill(pixelArray, x + 1, y, seedFound);
// 		find_fill(pixelArray, x, y - 1, seedFound);
// 		find_fill(pixelArray, x, y + 1, seedFound);
// 	}
// }

void fill_below(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double y0, double radius, int x, int y){
	int i;
	for (i = y0; i < y; ++i) {
		pixelArray[x][i] = 255;	
	}
}

void fill_above(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double y0, double radius, int x, int y){
	int i;
	for (i = y0; i > y; --i) {
		pixelArray[x][i] = 255;	
	}
}

void draw_circle(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double x0, double y0, double radius)
{
    int x = radius;
    int y = 0;
    int err = 0;
    int pixelX, pixelY;

    while (x >= y)
    {
      
      pixelX = x0 + x;
    	pixelY = y0 + y;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_below(pixelArray, y0, radius, pixelX, pixelY);
    	}
      
      pixelX = x0 + y;
    	pixelY = y0 + x;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_below(pixelArray, y0, radius, pixelX, pixelY);
    	}
      
      pixelX = x0 - y;
    	pixelY = y0 + x;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_below(pixelArray, y0, radius, pixelX, pixelY);
    	}
      
      pixelX = x0 - x;
    	pixelY = y0 + y;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_below(pixelArray, y0, radius, pixelX, pixelY);
    	}
      
      pixelX = x0 - x;
    	pixelY = y0 - y;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_above(pixelArray, y0, radius, pixelX, pixelY);
    	}
      
      pixelX = x0 - y;
    	pixelY = y0 - x;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_above(pixelArray, y0, radius, pixelX, pixelY);
    	}
      
      pixelX = x0 + y;
    	pixelY = y0 - x;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_above(pixelArray, y0, radius, pixelX, pixelY);
    	}       
			
			pixelX = x0 + x;
    	pixelY = y0 - y;
    	if (is_bounded_filled(pixelX, pixelY, pixelArray)) {
    		pixelArray[pixelX][pixelY] = (radius - err) / radius * 255;
    		fill_above(pixelArray, y0, radius, pixelX, pixelY);
    	}

        y += 1;
        err += 1 + 2*y;
        if (2*(err-x) + 1 > 0)
        {
            x -= 1;
            err += 1 - 2*x;
        }
    }
    // if (is_bounded_filled(x0, y0, pixelArray)) {
    // 	flood_fill(pixelArray, x0, y0);
    // }
    

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

void draw_line(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double brushRadius, int startX, int startY, int endX, int endY) {
	double slope = (double)(endY - startY) / (endX - startX);
	double b = calculate_b(startX, startY, slope);
	//if abs(slope) is less than 1, do operations on x, more than 1, y
	//if start of point is less that end, increment, otherwise decrement
	int i;
	if (abs(slope) < 1) {
		if (startX >= endX) {
			if (startX == endX){
				for(i = startY; i <= endY; ++i){
					draw_circle(pixelArray, startX, i, brushRadius);
				}
			} else {
				for (i = startX; i >= endX; --i) {
					double y = calculate_other_coordinate('y', i, slope, b);
					draw_circle(pixelArray, i, round(y), brushRadius);
				}
			}
		} else {
			for (i = startX; i <= endX; ++i) {
				double y = calculate_other_coordinate('y', i, slope, b);
				draw_circle(pixelArray, i, round(y), brushRadius);
			}
		}
	} else {
		if (startY >= endY) {
			if (startY == endY) {
				for(i = startX; i <= endX; ++i){
					draw_circle(pixelArray, i, startY, brushRadius);
				}
			} else {
				for (i = startY; i >= endY; --i) {
					double x = calculate_other_coordinate('x', i, slope, b);
					draw_circle(pixelArray, round(x), i, brushRadius);
				}
			}
		} else {
			for (i = startY; i <= endY; ++i) {
				double x = calculate_other_coordinate('x', i, slope, b);
				draw_circle(pixelArray, round(x), i, brushRadius);
			}
		}	
	}
}

//Structure for TIFF Header
struct header {
		uint16_t endian;
		uint16_t magicNumber;
		uint32_t ifdOffset;
};

//Structure for TIFF Tags
struct tag {
		uint16_t tagHeader;
		uint16_t dataType;
		uint32_t numValues;
		uint32_t value;
};

//Generates a tiff header with constants and the given pixel count
struct header generate_tiff_header(unsigned int pixelCount){
	struct header tiffHeader;
	tiffHeader.endian = HEADER_ENDIAN_LITTLE;
	tiffHeader.magicNumber = HEADER_MAGIC_NUMBER;
	tiffHeader.ifdOffset = pixelCount + 8;
	return tiffHeader;
}

void write_file(unsigned char *buffer, unsigned int imageNumber, 
		unsigned int label, size_t fileLength){
	char fileName[50];
	sprintf(fileName, "UNIPEN/image%d(%d).tiff", imageNumber, label);
	FILE *file = fopen(fileName, "wb");
	fwrite(buffer, sizeof(unsigned char), fileLength, file);
	fclose(file); 
}

//Generates the TIFF file at a given image number; dependent on write_file
void generate_tiff_file(struct header header, struct tag tags[11],
	 	unsigned char pixelDataBuffer[500][500], unsigned int pixelCount, 
	 	unsigned int charLabel, unsigned int imageNumber){
	
	uint16_t numTags = NUM_TAGS;
	uint32_t tiffEnd = TIFF_END;
	size_t fileLength = sizeof(struct header) +
	                    pixelCount +
	                    numTags * sizeof(struct tag) +
	                    sizeof(numTags)+
	                    sizeof(tiffEnd);
	//allocate memory for the TIFF data
	unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * 
			fileLength);
	//create pointer to mark the next address to write to
	unsigned char *marker = buffer;
	//copy header
	memcpy(marker, &header, sizeof(struct header));
	marker += sizeof(struct header);
	//copy pixel data
	int i;
	for(i = 0; i < VECTOR_DIMMENSION; ++i) {
		memcpy(marker, pixelDataBuffer[i], VECTOR_DIMMENSION);
		marker += VECTOR_DIMMENSION;
	}
	//copy TIFF footer
	memcpy(marker, &numTags, sizeof(numTags));
	marker += sizeof(numTags);
	//copy all tags

	for(i = 0; i < numTags; ++i){
		memcpy(marker, &tags[i], sizeof(struct tag));
		marker += sizeof(struct tag);
	}
	//copy end bytes
	memcpy(marker, &tiffEnd, sizeof(tiffEnd));
	//write the actual file
	write_file(buffer, imageNumber, charLabel, fileLength);
	//free the memory
	free(buffer);
}

void draw_pixel_array(int *coordinates, size_t numCoordinates, 
	unsigned int charLabel, unsigned int imageNumber){
	unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION] = {0};
	// draw_circle(pixelArray, 10, 10, 4);
	// draw_circle(pixelArray, 12, 12, 4);
	// draw_circle(pixelArray, 13, 13, 4);
	// draw_circle(pixelArray, 10, 17, 7);
	int i, j;

	for(i = 0; i < numCoordinates - 3; i += 2) {
		draw_line(pixelArray, 10, coordinates[i], coordinates[i + 1], 
				coordinates[i + 2], coordinates[i + 3]);
	}

	// draw_line(pixelArray, 4, 4, 7, 23, 15);
	// for (i = 0; i < VECTOR_DIMMENSION; ++i) {
	// 	for (j = 0; j < VECTOR_DIMMENSION; ++j) {
	// 		printf("%d ", (int)pixelArray[i][j]);
	// 	}
	// 	printf("\n");
	// }

	int pixelCount = VECTOR_DIMMENSION * VECTOR_DIMMENSION;

	//generate header
	struct header tiffHeader = generate_tiff_header(pixelCount);

	//generate tags
	enum dataTypes {TYPE_BYTE = 1, TYPE_ASCII, TYPE_SHORT, TYPE_LONG,
		TYPE_RATIONAL};
	struct tag tiffTags[12] = {
		{TAG_WIDTH,             TYPE_LONG,  1, VECTOR_DIMMENSION}, 
		{TAG_HEIGHT,            TYPE_LONG,  1, VECTOR_DIMMENSION},
		{TAG_BITS_PER_SAMPLE,   TYPE_SHORT, 1, 8},
		{TAG_COMPRESSION,       TYPE_SHORT, 1, 1},
		{TAG_INTERPRETATION,    TYPE_SHORT, 1, 0},
		{TAG_STRIP_OFFSETS,     TYPE_LONG,  1, 8},
		{TAG_ROWS_PER_STRIP,    TYPE_SHORT, 1, VECTOR_DIMMENSION},
		{TAG_STRIP_BYTE_COUNTS, TYPE_LONG,  1, pixelCount},
		{TAG_X_RESOLUTION,      TYPE_LONG,  1, 1},
		{TAG_Y_RESOLUTION,      TYPE_LONG,  1, 1},
		{TAG_RESOLUTION_UNIT,   TYPE_SHORT, 1, 1},
		{TAG_ORIENTATION,       TYPE_SHORT, 1, 8}
	};

	//create the actual file
	generate_tiff_file(tiffHeader, tiffTags, pixelArray, pixelCount, charLabel, 
		imageNumber);
	


}

size_t load_and_plot_coordinates(char *token, unsigned int charLabel, 
	unsigned int imageNumber) {
	size_t arraySize = 0;
	int *coordinates = malloc(10000 * sizeof(int));

	char *coordinate = strtok(token, " ");
	int i;
	for (i = 0; i < 5; ++i){
		coordinate = strtok(NULL, " ");
	}
	coordinate = strtok(token, " ");
	while (coordinate != NULL) {
		if (coordinate == ".DT") {
			coordinate = strtok(NULL, " ");
		} else if (atoi(coordinate) != 0) {
			coordinates[arraySize++] = atoi(coordinate);  
		} else {
		}
		coordinate = strtok(NULL, " ");
	}

	draw_pixel_array(coordinates, arraySize, charLabel, imageNumber);

	free(coordinates);
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
	token = strtok(data, "?");

	int firstTime = 1;
	int imageNumber = 0;
	while(token != NULL){
	// for (i = 0; i < 9; ++i) {
		//search each token for PEN_DOWN
		// printf("%d\n", strlen(token));
		unsigned int charLabel = 0;

		// if (strstr(token, SEGMENT) != NULL) {
		// 	charLabel = atoi(strtok(token, "\""));
  // 		charLabel = atoi(strtok(NULL, "\""));	
		// }

		if (strstr(token, PEN_DOWN) != NULL) {
			// printf("%d\n", strlen(token));
			size_t arraySize = load_and_plot_coordinates(token, charLabel, 
					imageNumber++);
			// printf("%d\n", arraySize);
		// //if PEN_DOWN, parse token into array of values
		}
		// printf("%d\n", data);
		if (firstTime == 1) {
			data += (strlen(token) + 1) * sizeof(char);
		} else {
			data += (strlen(token) + 1) * sizeof(char);
		}

		// printf("%d\n", data);		
		
		//create new array
		//itterate through coordinates and call draw line between every two
		token = strtok(data, "?");
		// printf("%s\n", token);

		if(firstTime == 1){
			firstTime = 0;
		}
	}


}
