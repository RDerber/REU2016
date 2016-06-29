#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
  
#define PEN_UP                        "PEN_UP"
#define PEN_DOWN                    "PEN_DOWN"
#define INCLUDE                      "INCLUDE"
#define LEXICON                      "LEXICON"
#define COMMENT                      "COMMENT"
#define SEGMENT                      "SEGMENT"
#define HIERARCHY                  "HIERARCHY"

#define FILE_PATH         "pendigits-orig.tes"

#define VECTOR_DIMMENSION                  500
#define IMAGE_DIMMENSION                    28
#define SQRT_2                          1.4142
#define NUM_IMAGES                        4256


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

#define NUM_TAGS                            11
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


///Checks if the pixel at position x, y is out of bounds or is already black 
int is_bounded_filled(int x, int y, 
		unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION]) {
	if (x >= VECTOR_DIMMENSION || y >= VECTOR_DIMMENSION || 
  		x < 0 || y < 0 || pixelArray[x][y] == 255) {
		return 0;
	} else {
		return 1; 
	}
}


//Fills the pixels below a given pixel until its center Y is reached
void fill_below(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double y0, double radius, int x, int y){
	int i;
	for (i = y0; i < y; ++i) {
		pixelArray[x][i] = 255;	
	}
}

//Fills the pixels above a given pixel until its center Y is reached
void fill_above(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double y0, double radius, int x, int y){
	int i;
	for (i = y0; i > y; --i) {
		pixelArray[x][i] = 255;	
	}
}


//Draws the outer points of a circle using the midpoint circle algorithm given its center
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
}

//Helper function to calculate b of y = mx + b given a coordinate and slope
double calculate_b(int x, int y, double slope) {
	return y - slope * x;
}

//Helper function to calculate the other coordinate of a coordinate pair given
//the line equation, the coordinate, and the return type ('x' or 'y'). 
double calculate_other_coordinate(char XY, int coordinate, double slope, 
		double b) {
	if(XY == 'x'){
		return (coordinate - b) / slope;
	} else {
		return coordinate * slope + b;
	}
}


//Given the coordinates of two endpoints, finds the y = mx + b equation for the 
//line and, draws dots for every pixel in the relatively less constrained 
//dimmension
//X and y flipped to make image appear correct orientation
void draw_line(unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION], 
		double brushRadius, int startY, int startX, int endY, int endX) {
	double slope = (double)(endY - startY) / (endX - startX);
	double b = calculate_b(startX, startY, slope);

	//if abs(slope) is less than 1, do operations on x, more than 1, y
	//if start of point is less that end, increment, otherwise decrement 
	int i;
	if (abs(slope) < 1) {
		if (startX >= endX) {
			for (i = startX; i >= endX; --i) {
				double y = calculate_other_coordinate('y', i, slope, b);
				if (startY == endY) {
					double y = startY;
				}
				draw_circle(pixelArray, VECTOR_DIMMENSION - i, round(y), brushRadius);
			}
		} else {
			for (i = startX; i <= endX; ++i) {
				double y = calculate_other_coordinate('y', i, slope, b);
				if (startY == endY) {
					double y = startY;
				}
				draw_circle(pixelArray, VECTOR_DIMMENSION - i, round(y), brushRadius);
			}
		}
	} else {
		if (startY >= endY) {
			for (i = startY; i >= endY; --i) {
				double x;
				if (startX == endX) {
					x = startX;
				} else {
					x = calculate_other_coordinate('x', i, slope, b);
				}
				draw_circle(pixelArray, VECTOR_DIMMENSION - round(x), i, brushRadius);
			}
		} else {
			for (i = startY; i <= endY; ++i) {
				double x;
				if (startX == endX) {
					x = startX;
				} else {
					x = calculate_other_coordinate('x', i, slope, b);
				}
				draw_circle(pixelArray, VECTOR_DIMMENSION - round(x), i, brushRadius);
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


//Writes all image files to a directory named UNIPEN and labels them according
//to which image they are in the UNIPEN file
void write_file(unsigned char *buffer, unsigned int imageNumber, 
		unsigned int label, size_t fileLength){
	char fileName[50];
	sprintf(fileName, "UNIPEN/image%d(%d).tiff", imageNumber, label);
	FILE *file = fopen(fileName, "wb");
	fwrite(buffer, sizeof(unsigned char), fileLength, file);
	fclose(file); 
}


//Generates the TIFF file at a given image number; dependent on write_file
size_t generate_tiff_file(struct header header, struct tag tags[11],
	 	unsigned char pixelDataBuffer[500][500], unsigned int pixelCount, 
	 	unsigned char **fileMarker){
	
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
	//copy the single TIFF file to the current marker in the buffer of TIFF files
	memcpy(*fileMarker, buffer, fileLength);
	//free the memory
	free(buffer);

	return fileLength;
}


//Creates the actual byte array and draws a line between every sequential pair 
//of coordinates in the coordinates buffer. Then creates a TIFF header and tags 
//based on the dimmensions of the image, and creates the actual TIFF file.
size_t draw_pixel_array(int *coordinates, size_t numCoordinates, 
		unsigned char **marker, struct header tiffHeader, struct tag tiffTags[11]){
	unsigned char pixelArray[VECTOR_DIMMENSION][VECTOR_DIMMENSION] = {0};
	int i;
	if (numCoordinates > 4) {
		for(i = 0; i < numCoordinates - 3; i += 2) {
			draw_line(pixelArray, 10, coordinates[i], coordinates[i + 1], 
					coordinates[i + 2], coordinates[i + 3]);
		}
	}

	//create the actual file
	size_t fileLength = generate_tiff_file(tiffHeader, tiffTags, pixelArray, 
			VECTOR_DIMMENSION * VECTOR_DIMMENSION, marker);

	return fileLength;
}


size_t load_and_plot_coordinates(char *token, unsigned char **marker, 
		struct header tiffHeader, struct tag tiffTags[11]) {

	size_t arraySize = 0;
	int *coordinates = malloc(10000 * sizeof(int));

	char *coordinate = strtok(token, " ");
	while (coordinate != NULL) {
		if (atoi(coordinate) != 0) {
			coordinates[arraySize++] = atoi(coordinate);  
		}
		coordinate = strtok(NULL, " ");
	}

	size_t fileLength = draw_pixel_array(coordinates, arraySize, marker, 
			tiffHeader, tiffTags);
	free(coordinates);

	return fileLength;
}


size_t generate_tiff_buffer(char *data, int numImages, 
		unsigned char **tiffBuffer, size_t TIFFLength, struct header tiffHeader,
		struct tag tiffTags[11]){
	char *token = malloc(sizeof(char) * 1000);
	token = strtok(data, ".");
	size_t bufferLength = 0;
	int firstTime = 1;
	unsigned char *marker = *tiffBuffer;
	int i;
	for (i = 0; i != numImages;) {
		if (strstr(token, PEN_DOWN) != NULL) {
			size_t fileLength = load_and_plot_coordinates(token, &marker, tiffHeader, 
					tiffTags);
			bufferLength += fileLength; 
			marker += fileLength;
			++i;
		}

		if (firstTime == 1) {
			data += (strlen(token) + 2) * sizeof(char);
		} else {
			data += (strlen(token) + 1) * sizeof(char);
		}
		
		token = strtok(data, ".");

		if(firstTime == 1){
			firstTime = 0;
		}
	}
	free(token);
	return bufferLength;
}

void run_transformations(char *data, int numImages) {
		int pixelCount = VECTOR_DIMMENSION * VECTOR_DIMMENSION;

	//generate header
	struct header tiffHeader = generate_tiff_header(pixelCount);

	//generate tags
	enum dataTypes {TYPE_BYTE = 1, TYPE_ASCII, TYPE_SHORT, TYPE_LONG,
		TYPE_RATIONAL};
	struct tag tiffTags[11] = {
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
		{TAG_RESOLUTION_UNIT,   TYPE_SHORT, 1, 1}
	};

	size_t TIFFLength = sizeof(struct header) +
	                    VECTOR_DIMMENSION * VECTOR_DIMMENSION +
	                    NUM_TAGS * sizeof(struct tag) +
	                    sizeof(uint16_t)+
	                    sizeof(uint32_t);
	unsigned char *tiffBuffer = (unsigned char *)malloc(numImages * TIFFLength);

	size_t bufferLength = generate_tiff_buffer(data, numImages, &tiffBuffer, 
			TIFFLength, tiffHeader, tiffTags);

	free(tiffBuffer);
}

int sort_compare(const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

double calc_avg_k_lowest_runs(int **array, int numRuns, int k) {
	qsort(*array, numRuns, sizeof(int), sort_compare);
	int i, sum = 0;
	for (i = 0; i < k; ++i) {
		sum += *(*array + i);
		// printf("%d\n", sum);
	}
	return (double) sum / k;
}

void write_doubles_to_csv_file(double *values, int numValues, char *name) {
	char fileName[50];
	sprintf(fileName, "%s.csv", name);
	FILE *file = fopen(fileName, "w+");
	int i;
	printf("reached\n");
	for (i = 0; i < numValues; ++i) {
		fprintf(file,"%d, %f\n", i + 1, values[i]);
		printf("%d %f\n", i, values[i]);
	}
	printf("reached\n");
	fclose(file);
	
}


int main(int argc, char **argv){
	int numImages = 1;

	if (atoi(argv[1]) > 4256 || atoi(argv[1]) < 1) {
		printf("%s\n", "Invalid Number of Images");
		printf("%s\n", argv[1]);
	} else {
		numImages = atoi(argv[1]);
	}

	char *data;
	size_t fileLength = load_upen_to_buffer(FILE_PATH, &data);

	// int pixelCount = VECTOR_DIMMENSION * VECTOR_DIMMENSION;

	// //generate header
	// struct header tiffHeader = generate_tiff_header(pixelCount);

	// //generate tags
	// enum dataTypes {TYPE_BYTE = 1, TYPE_ASCII, TYPE_SHORT, TYPE_LONG,
	// 	TYPE_RATIONAL};
	// struct tag tiffTags[11] = {
	// 	{TAG_WIDTH,             TYPE_LONG,  1, VECTOR_DIMMENSION}, 
	// 	{TAG_HEIGHT,            TYPE_LONG,  1, VECTOR_DIMMENSION},
	// 	{TAG_BITS_PER_SAMPLE,   TYPE_SHORT, 1, 8},
	// 	{TAG_COMPRESSION,       TYPE_SHORT, 1, 1},
	// 	{TAG_INTERPRETATION,    TYPE_SHORT, 1, 0},
	// 	{TAG_STRIP_OFFSETS,     TYPE_LONG,  1, 8},
	// 	{TAG_ROWS_PER_STRIP,    TYPE_SHORT, 1, VECTOR_DIMMENSION},
	// 	{TAG_STRIP_BYTE_COUNTS, TYPE_LONG,  1, pixelCount},
	// 	{TAG_X_RESOLUTION,      TYPE_LONG,  1, 1},
	// 	{TAG_Y_RESOLUTION,      TYPE_LONG,  1, 1},
	// 	{TAG_RESOLUTION_UNIT,   TYPE_SHORT, 1, 1}
	// };

	// size_t TIFFLength = sizeof(struct header) +
	//                     VECTOR_DIMMENSION * VECTOR_DIMMENSION +
	//                     NUM_TAGS * sizeof(struct tag) +
	//                     sizeof(uint16_t)+
	//                     sizeof(uint32_t);
	// unsigned char *tiffBuffer = (unsigned char *)malloc(numImages * TIFFLength);

	// printf("TIFFBUFFER %d\n", tiffBuffer);

	// size_t bufferLength = generate_tiff_buffer(data, numImages, &tiffBuffer, 
	// 		TIFFLength, tiffHeader, tiffTags);

	double *timingArray;
	timingArray = (double *) malloc(numImages * sizeof(int));

	struct timeval startTime, endTime;

	int i, j;
	int numRuns = 50;
	for (i = 0; i < numImages; ++i) {
		int *runsArray = (int *) malloc(numRuns * sizeof(int));
		for (j = 0; j < numRuns; ++j) {	
			gettimeofday(&startTime, NULL);
			run_transformations(data, i + 1);
			// sleep(1);
			gettimeofday(&endTime, NULL);
			runsArray[j] = (endTime.tv_usec - startTime.tv_usec) + 
					(endTime.tv_sec - startTime.tv_sec) * 1000000;
			// printf("took%d\n", runsArray[j]);
		}
		int k = 30;
		timingArray[i] = calc_avg_k_lowest_runs(&runsArray, numRuns, k);
		free(runsArray);
		// printf("%f\n", timingArray[i]);
	}

	// printf("%f\n", timingArray[0]);
	printf("LASTELEMENT %f", timingArray[numImages - 1]);
	write_doubles_to_csv_file(timingArray, numImages, "test1");
	// run_transformations(data, numImages);



	// FILE *file = fopen("test", "wb");
	// fwrite(tiffBuffer, sizeof(unsigned char), bufferLength, file);
	// fclose(file); 
	return 0;
}
