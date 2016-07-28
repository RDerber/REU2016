/**
 * Transforms an opdigits text-based file format (with input filepath defined 
 * as a constant string) into individual TIFF images. The images are named 
 * "image" + IMAGE_NUMBER + (CHARACTER) + ".tiff" and exported into a folder 
 * named "TIFF". All of the pixel data in the optdigits input file will be 
 * transformed into TIFF images.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define INPUT_FILEPATH    "optdigits-orig.tra"

// FILE FORMAT SPECIFICATIONS
#define LINE_OFFSET                          3
#define MAX_LINE_SIZE                       60

// TIFF HEADER CODES
#define HEADER_ENDIAN_LITTLE            0x4949
#define HEADER_ENDIAN_BIG               0x4D4D
#define HEADER_MAGIC_NUMBER             0x002A

// TIFF TAG ID CODES                           
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

// Loads input file at the provided filepath into the provided buffer
// Returns the size of the input file
size_t load_input_buffer(char *filepath, unsigned char **buffer) {
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
	
	printf("%d\n", (int)fileSize);
	return fileSize;
}


// Structure for TIFF Header
struct header {
		uint16_t endian;
		uint16_t magicNumber;
		uint32_t ifdOffset;
};

// Structure for TIFF Tags
struct tag {
		uint16_t tagHeader;
		uint16_t dataType;
		uint32_t numValues;
		uint32_t value;
};


// Generates a tiff header with constants and the given pixel count
struct header generate_tiff_header(int pixelCount){
	struct header tiffHeader;
	tiffHeader.endian = HEADER_ENDIAN_LITTLE;
	tiffHeader.magicNumber = HEADER_MAGIC_NUMBER;
	tiffHeader.ifdOffset = pixelCount + 8;
	return tiffHeader;
}


// Writes TIFF file with given labels using the buffer of data
void write_file(unsigned char *buffer, unsigned int imageNumber, 
		unsigned int label, size_t fileLength){
	char fileName[50];
	sprintf(fileName, "TIFF/image%d(%d).tiff", imageNumber, label);
	FILE *file = fopen(fileName, "wb");
	fwrite(buffer, sizeof(unsigned char), fileLength, file);
	fclose(file); 
}


// Copies the tiff data at the position of the given output buffer. Anticipates 
// that the output buffer position provided be advanced with each tiff file as 
// to append to the existing buffer data. Optionally exports the tiff data as a 
// tiff file. Returns the size of the tiff file written to the buffer.
size_t generate_and_append_tiff(unsigned char *pixelData, 
        unsigned char **outputBuffer, int width, int height, 
        struct header header, struct tag tags[11], int imageNumber, int label) {
   	int pixelCount = width * height;
   	uint16_t numTags = NUM_TAGS;
	uint32_t tiffEnd = TIFF_END;
	size_t fileLength = sizeof(struct header) +
	                    pixelCount +
	                    numTags * sizeof(struct tag) +
	                    sizeof(numTags)+
	                    sizeof(tiffEnd);
	// create pointer to mark the next address to write to
	unsigned char *marker = *outputBuffer;
	// copy header
	memcpy(marker, &header, sizeof(struct header));
	marker += sizeof(struct header);
	// copy pixel data
	memcpy(marker, pixelData, pixelCount);
	marker += pixelCount;
	// copy TIFF footer
	memcpy(marker, &numTags, sizeof(numTags));
	marker += sizeof(numTags);
	// copy all tags
	int i;
	for(i = 0; i < numTags; ++i){
		memcpy(marker, &tags[i], sizeof(struct tag));
		marker += sizeof(struct tag);
	}
	// copy end bytes
	memcpy(marker, &tiffEnd, sizeof(tiffEnd));
	marker += sizeof(tiffEnd);
	// write the actual file
	write_file(*outputBuffer, imageNumber, label, fileLength);
	return fileLength; 
}


// Interprets a string of '0' and '1' as a sequence of bytes (0x00 and 0xff). 
// Writes the hex values as chars to the provided buffer.
void append_pixel_data(unsigned char **marker, char *token, int width) {
    unsigned char black[] = {0xff};
    unsigned char white[] = {0x00};
    int i;
    for (i = 0; i < width; ++i) {
        if (*(token + i) == '1') {
            memcpy((*marker + i), black, sizeof(char));
        } else if (*(token + i) == '0') {
            memcpy((*marker + i), white, sizeof(char));
        }
        printf("%c ", *(token + i));
    }
    printf("\n");
}


// Takes in a pointer to the beginning of a pixel array wihtin the input file 
// buffer. Interprets the data and writes to a pixelData buffer; passes the 
// data to be transformed to a tiff file. Returns the size of the character data 
// read from the input file.
size_t append_pixels_as_tiff(unsigned char *inputBuffer, 
        unsigned char **outputBuffer, int width, int height, 
        struct header header, struct tag tags[11], int imageNumber) {
    unsigned char *pixelData = malloc(width * height * sizeof(unsigned char));
    unsigned char *marker = pixelData;
    
    size_t pixelBlockSize = 0;
    
    char *lineDelimiters = "\n";

    char *token;
    size_t tokenSize;
    int i;
    for (i = 0; i < height; ++i) {
        token = strtok(inputBuffer, lineDelimiters);
        tokenSize = strlen(token);
        append_pixel_data(&marker, token, width);
        marker += tokenSize;
        inputBuffer += tokenSize + 1;
        pixelBlockSize += tokenSize + 1;
    }
    
    token = strtok(inputBuffer, lineDelimiters);
    tokenSize = strlen(token);
    int label = atoi(token);
    inputBuffer += tokenSize + 1;
    pixelBlockSize += tokenSize + 1;
    
    printf("%d\n", (int)pixelBlockSize);
    
    generate_and_append_tiff(pixelData, outputBuffer, width, height, header, 
            tags, imageNumber, label);
    
    free(pixelData);
    
    return pixelBlockSize;
}


// Prepares the output buffer, tiff tags, and tiff header. Iterates through the
// blocks of character data in the input file buffer and passes them to be 
// appended onto the output buffer as tiff files.
void generate_output_buffer(unsigned char *inputBuffer, 
        unsigned char **outputBuffer, int width, int height, int numChars) {
	uint16_t numTags = NUM_TAGS;
	uint32_t tiffEnd = TIFF_END;
    int pixelCount = width * height;
	size_t fileLength = sizeof(struct header) +
                        pixelCount +
                        numTags * sizeof(struct tag) +
                        sizeof(numTags)+
                        sizeof(tiffEnd);
    *outputBuffer = malloc(fileLength * numChars * sizeof(unsigned char));
    
    // generate header
	struct header tiffHeader = generate_tiff_header(pixelCount);

	// generate tags
	enum dataTypes {TYPE_BYTE = 1, TYPE_ASCII, TYPE_SHORT, TYPE_LONG,
		TYPE_RATIONAL};
	struct tag tiffTags[11] = {
		{TAG_WIDTH,             TYPE_LONG,  1, width}, 
		{TAG_HEIGHT,            TYPE_LONG,  1, height},
		{TAG_BITS_PER_SAMPLE,   TYPE_SHORT, 1, 8},
		{TAG_COMPRESSION,       TYPE_SHORT, 1, 1},
		{TAG_INTERPRETATION,    TYPE_SHORT, 1, 0},
		{TAG_STRIP_OFFSETS,     TYPE_LONG,  1, 8},
		{TAG_ROWS_PER_STRIP,    TYPE_SHORT, 1, height},
		{TAG_STRIP_BYTE_COUNTS, TYPE_LONG,  1, pixelCount},
		{TAG_X_RESOLUTION,      TYPE_LONG,  1, 1},
		{TAG_Y_RESOLUTION,      TYPE_LONG,  1, 1},
		{TAG_RESOLUTION_UNIT,   TYPE_SHORT, 1, 1}
	};

    unsigned char *marker = *outputBuffer;
    int i;
    for (i = 0; i < numChars; ++i) {
        size_t pixelBlockSize = append_pixels_as_tiff(inputBuffer, &marker, 
                width, height, tiffHeader, tiffTags, i);
        inputBuffer += pixelBlockSize;
        marker += fileLength;
    }
}


// Given a string in the form "LABEL = VALUE", returns the integer value of 
// VALUE.
int return_field(char *token) {
	char *tokenDelimiters = " =";
	strtok(token, tokenDelimiters);
	return atoi(strtok(NULL, tokenDelimiters));
}


// Reads and stores speicifcations at the beginning of the file. Transforms the 
// character data to a TIFF format and appends it to the output buffer. 
void write_output_buffer(unsigned char *inputBuffer, 
        unsigned char **outputBuffer) {
    char *token;
    char *lineDelimiters = "\n";
    int tokenLength;
    
    // Ignore first 2 lines
    int i;    
    for (i = 0; i < 2; ++i) {
    	token = strtok(inputBuffer, lineDelimiters);
    	printf("%s\n", token);
    	inputBuffer += strlen(token) + 1;
    }
    inputBuffer++;
    
    // Read the width field
    token = strtok(inputBuffer, lineDelimiters);
    tokenLength = strlen(token) + 1;
    int width = return_field(token);
    inputBuffer += tokenLength;
    printf("width: %d\n", width);
    
    // Read the height field
    token = strtok(inputBuffer, lineDelimiters);
    tokenLength = strlen(token) + 1;
    int height = return_field(token);
    inputBuffer += tokenLength;
	printf("height: %d\n", height);
	
	// Ignore the next 3 lines
    for (i = 0; i < 3; ++i) {
    	token = strtok(inputBuffer, lineDelimiters);
    	inputBuffer += strlen(token) + 1;
    }
    
    // Read the number of characters field
    token = strtok(inputBuffer, lineDelimiters);
    tokenLength = strlen(token) + 1;
    int numChars = return_field(token);
    inputBuffer += tokenLength;
    printf("numChars: %d\n", numChars);
    
    // Ignore the next 12 lines
    for (i = 0; i < 12; ++i) {
    	token = strtok(inputBuffer, lineDelimiters);
    	inputBuffer += strlen(token) + 1;
    }
    
    // Begin interpretting data from the current position of inputBuffer
    generate_output_buffer(inputBuffer, outputBuffer, width, height, numChars);
}


void main() {
    unsigned char *inputBuffer;
    unsigned char *outputBuffer;
    
    size_t inputSize = load_input_buffer(INPUT_FILEPATH, 
            &inputBuffer);
            
    write_output_buffer(inputBuffer, &outputBuffer);
    
    free(inputBuffer);
    free(outputBuffer);
}