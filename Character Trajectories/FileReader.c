#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

size_t writeStringToBuffer(char *s, char *buffer, size_t space){
	unsigned int i, c;
  char hex[3];

	for (i = 0; i < strlen(s); i += 2) {
	    hex[0] = s[i];
	    hex[1] = s[i+1];
	    hex[2] = '\0';
	    sscanf(hex,"%X",&c);
	    buffer[space++] = c;
    }

   return space;
}

size_t writeValueToBuffer(int value, int size, char *buffer, size_t space){
	if (size >= 4){
		buffer[space++]=
			(char)((value & 0xff000000) / 16777216);
	}
	if (size >= 3){
		buffer[space++]= 
			(char)((value & 0x00ff0000) / 65536);
	}
	if (size >= 2){
		buffer[space++]= 
			(char)((value & 0x0000ff00) / 256);
	}
	if (size >= 1){
		buffer[space++]=
			(char)(value & 0x000000ff);
	}
	return space;

}

size_t writeTIFToBuffer(int width, int height, char pixelArray[][width],
 	char *buffer, size_t space){
	
	space = writeStringToBuffer("4d4d002a", buffer, space);
	

	int offset = width * height * 3 + 8;
	space = writeValueToBuffer(offset, 4, buffer, space);

	int i,j,k;
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			for(k = 0; k < 3; k++){
				buffer[space++] = pixelArray[i][j];
				printf("%c ", pixelArray[i][j]);
			}
		}
		//printf("\n");
	}

	//footer
	space = writeStringToBuffer("000e", buffer, space);

	//width tag
	space = writeStringToBuffer("0100000300000001", buffer, space);
	space = writeValueToBuffer(width, 2, buffer, space);
	space = writeStringToBuffer("0000", buffer, space);

	//height tag
	space = writeStringToBuffer("0101000300000001", buffer, space);
	space = writeValueToBuffer(height, 2, buffer, space);
	space = writeStringToBuffer("0000", buffer, space);

	//bits per sample
	space = writeStringToBuffer("0102000300000003", buffer, space);
	offset = width * height * 3 + 182;
	space = writeValueToBuffer(offset, 4, buffer, space);

	 /* Compression flag, short int */
	space = writeStringToBuffer("010300030000000100010000", buffer, space);

	 /* Photometric interpolation tag, short int */
	space = writeStringToBuffer("010600030000000100020000", buffer, space);

	 /* Strip offset tag, long int */
	space = writeStringToBuffer("011100040000000100000008", buffer, space);

	 /* Orientation flag, short int */
	space = writeStringToBuffer("011200030000000100010000", buffer, space);

	 /* Sample per pixel tag, short int */
	space = writeStringToBuffer("011500030000000100030000", buffer, space);

	 /* Rows per strip tag, short int */
	space = writeStringToBuffer("0116000300000001", buffer, space);
	space = writeValueToBuffer(height, 2, buffer, space);
	space = writeStringToBuffer("0000", buffer, space);

	 /* Strip byte count flag, long int */
	space = writeStringToBuffer("0117000400000001", buffer, space);
	offset = width * height * 3;
	space = writeValueToBuffer(offset, 4, buffer, space);

	 /* Minimum sample value flag, short int */
	space = writeStringToBuffer("0118000300000003", buffer, space);
	offset = width * height * 3 + 188;
	space = writeValueToBuffer(offset, 4, buffer, space);
	 /* Maximum sample value tag, short int */
	space = writeStringToBuffer("0119000300000003", buffer, space);
	offset = width * height * 3 + 194;
	space = writeValueToBuffer(offset, 4, buffer, space);

	 /* Planar configuration tag, short int */
	space = writeStringToBuffer("011c00030000000100010000", buffer, space);
	 /* Sample format tag, short int */
	space = writeStringToBuffer("0153000300000003", buffer, space);
	offset = width * height * 3 + 200;
	space = writeValueToBuffer(offset, 4, buffer, space);

	 /* End of the directory entry */
	space = writeStringToBuffer("00000000", buffer, space);

	 /* Bits for each colour channel */
	space = writeStringToBuffer("000800080008", buffer, space);

	 /* Minimum value for each component */
	space = writeStringToBuffer("000000000000", buffer, space);

	 /* Maximum value per channel */
	space = writeStringToBuffer("00ff00ff00ff", buffer, space);

	 /* Samples per pixel for each channel */
	space = writeStringToBuffer("000100010001", buffer, space);

	/* X resolution */
	space = writeStringToBuffer("011a000400000001", buffer, space);
	offset = 1;
	space = writeValueToBuffer(offset, 4, buffer, space);

	/* Y resolution */
	space = writeStringToBuffer("011b000400000001", buffer, space);
	offset = 1;
	space = writeValueToBuffer(offset, 4, buffer, space);

	return space;
}

int main(int argc, char **argv){

	FILE *inputFile;
	char *buffer;
	long fileLength;

	inputFile = fopen("t10k-images.idx3-ubyte", "rb");
	if(inputFile == NULL){
		printf("Cannot open inputFile \n");
		return 1;
	} 
	fseek(inputFile, 0, SEEK_END);
	fileLength = ftell(inputFile);
	rewind(inputFile);

	buffer = (char *)malloc((fileLength)*sizeof(char));
	fread(buffer, fileLength, 1, inputFile);
	fclose(inputFile);
	printf("File successfully read \n");

	const int numImages = (buffer[6] << 8) | (buffer[7]);
	const int numRows = buffer[11];
	const int numCols = buffer[15];

	printf("The number of images is %d\n", numImages);
	printf("The number of rows is %d\n", numRows);
	printf("The number of columns is %d\n", numCols);

	char pixelArray[numRows][numCols];

	int array = 0;
	if(argc == 2){
		if(atoi(argv[1]) && atoi(argv[1]) < numImages - 1){
			array = atoi(argv[1]);
		} else{
			printf("Incorrect input\n");
		}
	} else {
		printf("Incorrect number of arguments\n");
	}

	int arrayStart = ((numRows * numCols) * array) + 19;

	int currentRow = 1;
	int i;
	int j;

	int count = 0;
	for (i = 0; i < numRows; i++) {
		for (j = 0; j < numCols; j++){
			char rawVal = 0xff - buffer[count + arrayStart]; 
			pixelArray[i][j] = rawVal;
			//printf("%c \n", pixelArray[i][j]);
			unsigned int USInt = ((int)rawVal < 0) ? rawVal + 256 : rawVal;
			printf("%u ", USInt);
			count++;
		}
		//printf("\n");
	}

	FILE *outputFile;
	char *byteBuffer;
	size_t space = 0;

	outputFile = fopen("Char.tiff", "wb");

	byteBuffer = (char *)malloc(sizeof(char) * 3000);
	printf("DEBUG \n");
	space = writeTIFToBuffer(numCols, numRows, pixelArray, byteBuffer, space);
	printf("%d\n", space);
	
	printf("DEBUG \n");

	if (!fwrite(byteBuffer, sizeof(char), space, outputFile)) {
   
      int errnum = errno;
      fprintf(stderr, "Value of errno: %d\n", errno);
      perror("Error printed by perror");
      fprintf(stderr, "Error writing file: %s\n", strerror( errnum ));
   }

	fclose(outputFile);

	printf("DEBUG \n");

	return 0;
	
}

