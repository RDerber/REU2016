/** 
 * Modified PLT_TO_CSV program to test runtimes. Takes in one command 
 * line argument N, runs and times M (defined as constant) transformations of 
 * the first 0 to N GPS coordinates, and finds the average of the K (defined as 
 * constant) lowest times. Does not export the transofmred csv file. Exports a 
 * csv file with number of GPS coordinates vs time in microseconds. The csv file 
 * Will be named PLATFORM + "CoordTest" + NUMCOORDINATES + ".csv"; Platform is 
 * defined and should be changed according to where the transformation is run.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// NUMBER OF RUNS
#define M                                   50
// NUMBER OF LOWEST TIMES TO AVERAGE
#define K                                    5

// OUTPUT PLATFORMS
#define CLOUD                          "Cloud"
#define LOCAL                          "Local"

// SET PLATFORM
#define PLATFORM                         CLOUD

// TODO: allow for variable filepaths
#define INPUT_FILEPATH "000/20081023025304.plt"

#define ID "000"
#define LINE_OFFSET 6
#define MAX_LINE_SIZE 64

// Loads input file at the provided filepath into the provided buffer
// Returns the size of the input file
size_t load_csv_to_buffer(char *filepath, char **buffer) {
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


// Returns the number of lines(coordinates) in the provided buffer
int count_GPS_coordinates(char *buffer) {
    char *token;
    char *fileDelimiters = "\n";
    token = strtok(buffer, fileDelimiters);
    
    // Ignore headers/unimportant data
    int i;
    for (i = 0; i < LINE_OFFSET; ++i) {
        buffer += strlen(token) + 1;
        token = strtok(buffer, fileDelimiters);
    }
    
    int numCoordinates = 0;
    while (token != NULL) {
        buffer += strlen(token) + 1;
        token = strtok(buffer, fileDelimiters);
        numCoordinates++;
    }
    return numCoordinates;
}


// Appends the id and coordinates within a provided token to the output bufffer
// Returns the size of the appended line.
size_t input_coordinates(char *token, char **marker) {
    char *tokenDelimiters = ",";
    char *lattitude;
    char *longitude;
    char *id;
    
    lattitude = strtok(token, tokenDelimiters);
    size_t lattitudeSize = strlen(lattitude);
    longitude = strtok(NULL, tokenDelimiters);
    size_t longitudeSize = strlen(longitude);
    
    // Copy data in generalized format into output buffer
    memcpy(*marker, ID, strlen(ID) * sizeof(char));
    *marker += strlen(ID) * sizeof(char);
    memcpy(*marker, ",", sizeof(char));
    *marker += sizeof(char);
    memcpy(*marker, lattitude, lattitudeSize * sizeof(char));
    *marker += lattitudeSize * sizeof(char);
    memcpy(*marker, ",", sizeof(char));
    *marker += sizeof(char);
    memcpy(*marker, longitude, longitudeSize * sizeof(char));
    *marker += longitudeSize * sizeof(char);
    memcpy(*marker, "\n", sizeof(char));
    *marker += sizeof(char);
    
    return (strlen(ID) + lattitudeSize + longitudeSize + 3) * 
            sizeof(char);
}


// Converts the provided input file buffer into a generalized csv format and 
// writes it into the provided output buffer
// Returns the size of the output buffer
size_t convert_to_csv(char *inputBuffer, char **outputBuffer, int numToTransform) {
    char *fileDelimiters = "\n";
    char *token;

    token = strtok(inputBuffer, fileDelimiters);
    char *marker = *outputBuffer;
    
    // Ignore headers/unimportant data
    int i;
    for (i = 0; i < LINE_OFFSET; ++i) {
        inputBuffer += strlen(token) + 1;
        token = strtok(inputBuffer, fileDelimiters);
    }
    
    size_t fileSize = 0;
    while (token != NULL && numToTransform != 0) {
        inputBuffer += strlen(token) + 1;
        fileSize += input_coordinates(token, &marker);
        token = strtok(inputBuffer, fileDelimiters);
        numToTransform --;
    }
    return fileSize;
}


// Writes file of provided file size with contents of provided buffer
void write_csv_file(char * filepath, char *buffer, size_t fileSize) {
    FILE *file = fopen(filepath, "w");
    fwrite(buffer, fileSize, sizeof(char), file);
    fclose(file);
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


// Writes an array of doubles as pairs of indexes and their values to a csv file
void write_doubles_to_csv_file(double *values, int numValues, char *name) {
	char fileName[50];
	sprintf(fileName, "%sCoordTest%d.csv", name, numValues);
	FILE *file = fopen(fileName, "w+");
	int i;
	for (i = 0; i < numValues; ++i) {
		fprintf(file,"%d, %f\n", i + 1, values[i]);
 		printf("%d %f\n", i, values[i]);
	}
	fclose(file);
}


void main(int argc, char **argv){
    char *inputBuffer;
    char *outputBuffer;
    
    size_t inputSize = load_csv_to_buffer(INPUT_FILEPATH, 
            &inputBuffer);

    char *inputBufferCopy = malloc(inputSize);
	memcpy(inputBufferCopy, inputBuffer, inputSize);
    int numCoordinates = count_GPS_coordinates(inputBufferCopy);
    free(inputBufferCopy);
    
    int numImages = 1;
	if (atoi(argv[1]) > numCoordinates || atoi(argv[1]) < 1) {
		printf("%s\n", "Invalid Number of Images");
		printf("%s\n", argv[1]);
	} else {
		numImages = atoi(argv[1]);
	}
	
	 double *timingArray;
	timingArray = (double *) malloc(numImages * sizeof(double));

	struct timeval startTime, endTime;

	int i, j;
	int numRuns = M;
	int k = K;
	
	// time the transofrmation of all numbers of images up to numImages 
	for (i = 0; i < numImages; ++i) {
		int *runsArray = (int *) malloc(numRuns * sizeof(int));
		for (j = 0; j < numRuns; ++j) {	
		    char *inputBufferCopy = malloc(inputSize);
		    memcpy(inputBufferCopy, inputBuffer, inputSize);
		    outputBuffer = malloc(numCoordinates * MAX_LINE_SIZE * 
		            sizeof(char));
			gettimeofday(&startTime, NULL);
			convert_to_csv(inputBufferCopy, &outputBuffer, i + 1);
			gettimeofday(&endTime, NULL);
			runsArray[j] = (endTime.tv_usec - startTime.tv_usec) + 
					(endTime.tv_sec - startTime.tv_sec) * 1000000;
			free(inputBufferCopy);
			free(outputBuffer);
		}
		timingArray[i] = calc_avg_k_lowest_runs(&runsArray, numRuns, k);
		free(runsArray);
	}

	// write timing data (character to runtime) to csv file
	write_doubles_to_csv_file(timingArray, numImages, PLATFORM);
    
    free(inputBuffer);
}