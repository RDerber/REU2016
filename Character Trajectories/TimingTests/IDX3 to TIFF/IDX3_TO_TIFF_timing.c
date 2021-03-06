/** 
 * Modified IDX3_TO_TIFF program to test runtimes. Takes in one command line 
 * argument N, runs and times M (defined as constant) transformations of the 
 * first 0 to N images, and finds the average of the K (defined as constant) 
 * lowest times. Does not export any TIFF images. Exports a csv file with number 
 * of images vs time in microseconds. The csv file Will be named PLATFORM + 
 * "CharTest" + NUMIMAGES + ".csv"; Platform is defined and should be changed 
 * according to where the transformation is run.
 */
 
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

// NUMBER OF RUNS
#define M                                   50
// NUMBER OF LOWEST TIMES TO AVERAGE
#define K                                   30

// OUTPUT PLATFORMS
#define CLOUD                          "Cloud"
#define LOCAL                          "Local"

// SET PLATFORM
#define PLATFORM                         CLOUD

// IDX1-UBYTE SPECIFIED BYTE OFFSETS
#define IDX1_OFFSET_LABEL                    8

// IDX3-UBYTE SPECIFIED BYTE OFFSETS
#define IDX3_OFFSET_IMAGE_NUM                6
#define IDX3_OFFSET_ARRAY_START             16
#define IDX3_OFFSET_ROW_NUM                 11
#define IDX3_OFFSET_COL_NUM                 15

// LOCAL FILEPATHS
#define FILEPATH_IMG  "t10k-images.idx3-ubyte"
#define FILEPATH_LBL  "t10k-labels.idx1-ubyte"

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

// Loads the byte data of the .idx3-ubyte at the given filepath into the 
// given buffer and returns the length of the file(buffer)
size_t idx_load_to_buffer(char *filepath, unsigned char **buffer) {

	// open file
	FILE *file = fopen(filepath, "rb");
	if(file == NULL){
		printf("Cannot open file \n");
	} 

	// find the length of the file
	fseek(file, 0, SEEK_END);
	long fileLength = ftell(file);
	rewind(file);

	// allocate the correct amount of memory  
	*buffer = (unsigned char *)malloc(sizeof(unsigned char) * fileLength);
	
	// read the file into the buffer
	fread(*buffer, fileLength, 1, file);
	fclose(file);
	printf("File successfully read \n");

	return fileLength;
}


// Sets the pointer of a given buffer for pixel data to the beginning of actual
// pixel data within the given buffer holding the entire input file
void set_buffer_to_pixel_array(unsigned char **imageDataBuffer, 
			unsigned char **pixelDataBuffer, int imageNumber, 
			unsigned int pixelCount){

	// set the pointer to the correct offset within the input file buffer
	int arrayStart = ((pixelCount) * imageNumber) + IDX3_OFFSET_ARRAY_START;	
	*pixelDataBuffer = *imageDataBuffer + arrayStart;
}


// Reads a 2-byte int from 2 unsigned chars, given the offset from the high bit
unsigned int read_2byte_int(unsigned char *imageDataBuffer, 
		unsigned int offset){
	unsigned int numHighBit = *(imageDataBuffer + offset);
	unsigned int numLowBit = *(imageDataBuffer + offset + 1);
	return (numHighBit << 8 ) | (numLowBit);
}


// Finds the corresponding label value given the image number
unsigned int idx1_read_label(unsigned char *labelDataBuffer, 
		unsigned int imageNumber){
	return (unsigned int)*(labelDataBuffer + IDX1_OFFSET_LABEL + imageNumber);
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
struct header generate_tiff_header(unsigned int pixelCount){
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


// Generates the TIFF file at a given image number; dependent on write_file
void generate_tiff_file(struct header header, struct tag tags[11],
	 	unsigned char *pixelDataBuffer, unsigned char *labelDataBuffer,
	  unsigned int pixelCount, unsigned int imageNumber){
	
	uint16_t numTags = NUM_TAGS;
	uint32_t tiffEnd = TIFF_END;
	size_t fileLength = sizeof(struct header) +
	                    pixelCount +
	                    numTags * sizeof(struct tag) +
	                    sizeof(numTags)+
	                    sizeof(tiffEnd);
	// allocate memory for the TIFF data
	unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * 
			fileLength);
	// create pointer to mark the next address to write to
	unsigned char *marker = buffer;
	// copy header
	memcpy(marker, &header, sizeof(struct header));
	marker += sizeof(struct header);
	// copy pixel data
	memcpy(marker, pixelDataBuffer, pixelCount);
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
	// find the label based on the image number
	unsigned int label = idx1_read_label(labelDataBuffer, imageNumber);
	// write the actual file
	// write_file(buffer, imageNumber, label, fileLength);
	// free the memory
	free(buffer);
}


// Function passed into qsort to sort the runtimes
int sort_compare(const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

// Calculates the average of the k lowest runs in the provided array. Returns 
// that average as a double.
double calc_avg_k_lowest_runs(int **array, int numRuns, int k) {
	qsort(*array, numRuns, sizeof(int), sort_compare);
	int i, sum = 0;
	for (i = 0; i < k; ++i) {
		sum += *(*array + i);
	}
	return (double) sum / k;
}


// Writes the timing data to a csv file as pairs of index and value. 
void write_doubles_to_csv_file(double *values, int numValues, char *name) {
	char fileName[50];
	sprintf(fileName, "%sCharTest%d.csv", name, numValues);
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

void main(int argc, char **argv){
	unsigned char *imageDataBuffer;
	unsigned char *labelDataBuffer;
	unsigned char *pixelDataBuffer;

	// load the files into buffers and store the length
	size_t imageFileLength = idx_load_to_buffer(FILEPATH_IMG, &imageDataBuffer);
	size_t labelFileLength = idx_load_to_buffer(FILEPATH_LBL, &labelDataBuffer);
	
	unsigned int maxNumImages = read_2byte_int(imageDataBuffer, 
			IDX3_OFFSET_IMAGE_NUM);
	
	int numImages = 1;
	if (atoi(argv[1]) > maxNumImages || atoi(argv[1]) < 1) {
		printf("%s\n", "Invalid Number of Images");
		printf("%s\n", argv[1]);
	} else {
		numImages = atoi(argv[1]);
	}
	
	// Allocate memory for timing results
	double *timingArray;
	timingArray = (double *) malloc(numImages * sizeof(double));
	
	// Allocate memory for the number of coordinates
	double *coordinateCounts;
	coordinateCounts = (double *) malloc(numImages * sizeof(double));
	
	struct timeval startTime, endTime;

	int i, j;
	int numRuns = M;
	int k = K;
	// time the transofrmation of all numbers of images up to numImages 
	for (i = 0; i < numImages; ++i) {
		// allocate memory to hold all runtimes of a certain number of images
		int *runsArray = (int *) malloc(numRuns * sizeof(int));
		for (j = 0; j < numRuns; ++j) {	
			// Start time
			gettimeofday(&startTime, NULL);
			
			// store image specifications
			unsigned int height = *(imageDataBuffer + IDX3_OFFSET_ROW_NUM);
			unsigned int width = *(imageDataBuffer + IDX3_OFFSET_COL_NUM);
			unsigned int pixelCount = width * height;
		
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
				{TAG_STRIP_BYTE_COUNTS, TYPE_LONG,  1, height * width},
				{TAG_X_RESOLUTION,      TYPE_LONG,  1, 1},
				{TAG_Y_RESOLUTION,      TYPE_LONG,  1, 1},
				{TAG_RESOLUTION_UNIT,   TYPE_SHORT, 1, 1}
			};
		
			// create all the TIFF images in the idxs-ubyte file
			int k;
			for(k = 0; k < i; ++k){
				// set a pointer to the beginning of the correct set of pixel data 
				set_buffer_to_pixel_array(&imageDataBuffer, &pixelDataBuffer, i, 
						pixelCount);
				// create the actual file
				generate_tiff_file(tiffHeader, tiffTags, pixelDataBuffer, labelDataBuffer,
						pixelCount, k);
			}
			// End time
			gettimeofday(&endTime, NULL);
			runsArray[j] = (endTime.tv_usec - startTime.tv_usec) + 
					(endTime.tv_sec - startTime.tv_sec) * 1000000;
		}
		timingArray[i] = calc_avg_k_lowest_runs(&runsArray, numRuns, k);
		free(runsArray);
	}

	// write timing data (character to runtime) to csv file
	write_doubles_to_csv_file(timingArray, numImages, PLATFORM);
}

