#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//IDX3-UBYTE SPECIFIED BYTE OFFSETS
#define IDX3_OFFSET_IMAGE_NUM                6
#define IDX3_OFFSET_ARRAYSTART              19
#define IDX3_ROW_NUM_OFFSET                 11
#define IDX3_COL_NUM_OFFSET                 15

#define FILEPATH      "t10k-images.idx3-ubyte"

//TIFF HEADER CODES
#define HEADER_ENDIAN_LITTLE            0x4949
#define HEADER_ENDIAN_BIG               0x4D4D
#define HEADER_MAGIC_NUMBER             0x002a

#define NUM_TAGS                            11

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
#define TAG_RESOLUTION_UNIT             0x0152

//Loads the byte data of the .idx3-ubyte at the given filepath into the 
//given buffer and returns the length of the file(buffer)
size_t idx3ubyte_load_to_buffer(char *filepath, unsigned char **buffer) {

	//opens file
	FILE *file = fopen(filepath, "rb");
	if(file == NULL){
		printf("Cannot open file \n");
	} 

	//finds the length of the file
	fseek(file, 0, SEEK_END);
	long fileLength = ftell(file);
	rewind(file);

	//confirm file length
	printf("%d\n", fileLength);

	//allocates the correct amount of memory  
	*buffer = (unsigned char *)malloc(sizeof(unsigned char) * fileLength);
	
	//reads the file into the buffer
	fread(*buffer, fileLength, 1, file);
	fclose(file);
	printf("File successfully read \n");

	return fileLength;
}

//Sets the pointer of a given buffer for pixel data to the beginning of actual
//pixel data within the given buffer holding the entire input file
void set_buffer_to_pixel_array(unsigned char **inputBuffer, 
			unsigned char **pixelDataBuffer, int array, unsigned int arraySize){

	array = 0; // change

	//set the pointer to the correct offset within the input file buffer
	int arrayStart = ((arraySize) * array) + IDX3_OFFSET_ARRAYSTART;
	*pixelDataBuffer = *inputBuffer + arrayStart;
}

//reads the number of images from specified offset
unsigned int idx3ubyte_read_2byte_int(unsigned char *inputBuffer,
	 unsigned int offset){

	unsigned int numHighBit = *(inputBuffer + offset);
	unsigned int numLowBit = *(inputBuffer + offset + 1);
	return (numHighBit << 8 ) | (numLowBit);
}


struct header {
		uint16_t endian;
		uint16_t magicNumber;
		uint16_t ifdOffset;
};

struct tag {
		uint16_t tagHeader;
		uint16_t dataType;
		uint32_t numValues;
		uint32_t value;
};

struct header generate_tiff_header(unsigned int width, unsigned int height){
	struct header tiffHeader;
	tiffHeader.endian = HEADER_ENDIAN_LITTLE;
	tiffHeader.magicNumber = HEADER_MAGIC_NUMBER;
	tiffHeader.ifdOffset = width * height + 8;
	return tiffHeader;
}

struct tag * generate_tiff_tags(unsigned int width, unsigned int height){
	enum dataTypes {TYPE_BYTE = 1, TYPE_ASCII, TYPE_SHORT, TYPE_LONG,
			TYPE_RATIONAL};

	struct tag *tags = malloc(sizeof(struct tag) * NUM_TAGS); 

	tags = (struct tag[NUM_TAGS]){
		{TAG_WIDTH,             TYPE_LONG,  1, width}, 
		{TAG_HEIGHT,            TYPE_LONG,  1, height},
		{TAG_BITS_PER_SAMPLE,   TYPE_SHORT, 1, 8},
		{TAG_COMPRESSION,       TYPE_SHORT, 1, 1},
		{TAG_INTERPRETATION,    TYPE_SHORT, 1, 1},
		{TAG_STRIP_OFFSETS,     TYPE_LONG,  1, 8},
		{TAG_ROWS_PER_STRIP,    TYPE_SHORT, 1, height},
		{TAG_STRIP_BYTE_COUNTS, TYPE_LONG,  1, height * width},
		{TAG_X_RESOLUTION,      TYPE_LONG,  1, 1},
		{TAG_Y_RESOLUTION,      TYPE_LONG,  1, 1},
		{TAG_RESOLUTION_UNIT,   TYPE_SHORT, 1, 1}
	};

	return tags;
}

void generate_tiff_file(struct tag header, struct tag *tags,
	 	unsigned char *pixelData, unsigned int arraySize, unsigned int array){
	size_t fileLength = sizeof(header) +
	                    arraySize +
	                    NUM_TAGS * sizeof(struct tag) +
	                    sizeof(uint16_t) +
	                    sizeof(uint32_t);
	unsigned char *buffer = (char *)malloc()
}

int main(int argc, char **argv){
	
	unsigned char *inputBuffer;
	unsigned char *pixelDataBuffer;

	//load the file into inputBuffer and store the length
	size_t fileLength = idx3ubyte_load_to_buffer(FILEPATH, &inputBuffer);

	//confirm length
	printf("%d\n",fileLength );

	//stores image specifications
	unsigned int numImages = idx3ubyte_read_2byte_int(inputBuffer, 
			IDX3_OFFSET_IMAGE_NUM);
	unsigned int numRows = *(inputBuffer + IDX3_ROW_NUM_OFFSET);
	unsigned int numCols = *(inputBuffer + IDX3_COL_NUM_OFFSET);
	unsigned int arraySize = numRows * numCols;

	//set a pointer the correct set of pixel data 
	set_buffer_to_pixel_array(&inputBuffer, &pixelDataBuffer, 0, arraySize);
	
	//confirm values
	printf("%d\n", numImages);
	int i;
	for(i = 0; i < arraySize; ++i){
	  unsigned int testInt = pixelDataBuffer[i];
	  printf("%d ", testInt);	
	}

	struct header tiffHeader = generate_tiff_header(numCols, numRows);
	struct tag *tiffTags = generate_tiff_tags(numCols, numRows);
	

	return 0;
	
}

