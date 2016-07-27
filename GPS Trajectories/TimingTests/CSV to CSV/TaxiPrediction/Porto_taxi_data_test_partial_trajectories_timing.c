#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_FILEPATH "Porto_taxi_data_test_partial_trajectories.csv"
#define TIMING_FILEPATH_CLOUD  "CloudCharTest"
#define TIMING_FILEPATH_LOCAL  "LocalCharTest"
#define LINE_OFFSET 1
#define MAX_INPUT_LINE_SIZE 20000
#define MAX_OUTPUT_LINE_SIZE 64
#define NUM_COORDINATES 14438
#define NUM_FIELDS 9

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

// Appends the id and coordinates within a provided token to the output bufffer
// Returns the size of the appended line.
size_t input_coordinates(char *token, char **marker, int numToTransform) {
    char *tokenDelimiters = ",[]\" ";
    char *lattitude;
    char *longitude;
    char *id;
    
    // Read in useful data 
    id = strtok(token, tokenDelimiters);

    int i;
    for (i = 0; i < NUM_FIELDS - 2; ++i) {
        strtok(NULL, tokenDelimiters);   
    }
    lattitude = strtok(NULL, tokenDelimiters);
    longitude = strtok(NULL, tokenDelimiters);

    size_t size = 0;
    while(lattitude != NULL && longitude != NULL && numToTransform != 0) {
        
        memcpy(*marker, id, strlen(id) * sizeof(char));
        *marker += strlen(id) * sizeof(char);
        memcpy(*marker, ",", sizeof(char));
        *marker += sizeof(char);
        memcpy(*marker, lattitude, strlen(lattitude) * sizeof(char));
        *marker += strlen(lattitude) * sizeof(char);
        memcpy(*marker, ",", sizeof(char));
        *marker += sizeof(char);
        memcpy(*marker, longitude, strlen(longitude) * sizeof(char));
        *marker += strlen(longitude) * sizeof(char);
        memcpy(*marker, "\n", sizeof(char));
        *marker+= sizeof(char);
        
        size += (strlen(id) + strlen(lattitude) + strlen(longitude) + 3) * 
            sizeof(char);
        
        lattitude = strtok(NULL, tokenDelimiters);
        if (lattitude != NULL) {
            longitude = strtok(NULL, tokenDelimiters);
        }
        
        numToTransform--;
        // printf("%d\n", numToTransform);
    }
    
    return numToTransform;
}



// Converts the provided input file buffer into a generalized csv format and 
// writes it into the provided output buffer
// Returns the size of the output buffer
void convert_to_csv(char *inputBuffer, char **outputBuffer, 
		int numToTransform) {
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
        numToTransform = input_coordinates(token, &marker, numToTransform);
        token = strtok(inputBuffer, fileDelimiters);
    }
    // return fileSize;
}

// Writes file of provided file size with contents of provided buffer
void write_csv_file(char * filepath, char *buffer, size_t fileSize) {
    FILE *file = fopen(filepath, "w");
    fwrite(buffer, fileSize, sizeof(char), file);
    fclose(file);
}


int sort_compare(const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}


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
	sprintf(fileName, "%s%d.csv", name, numValues);
	FILE *file = fopen(fileName, "w+");
	int i;
	for (i = 0; i < numValues; ++i) {
		fprintf(file,"%d, %f\n", i + 1, values[i]);
		printf("%d %f\n", i, values[i]);
	}
	fclose(file);
}

void main(int argc, char **argv){
	int numImages = 1;
	if (atoi(argv[1]) > NUM_COORDINATES || atoi(argv[1]) < 1) {
		printf("%s\n", "Invalid Number of Images");
		printf("%s\n", argv[1]);
	} else {
		numImages = atoi(argv[1]);
	}
    char *inputBuffer;
    char *outputBuffer;
    
    size_t inputSize = load_csv_to_buffer(INPUT_FILEPATH, 
            &inputBuffer);
    
    
    double *timingArray;
	timingArray = (double *) malloc(numImages * sizeof(double));

	struct timeval startTime, endTime;

	int i, j;
	int numRuns = 50;
	int k = 5;
	
	// time the transofrmation of all numbers of images up to numImages 
	for (i = 0; i < numImages; ++i) {
		int *runsArray = (int *) malloc(numRuns * sizeof(int));
		for (j = 0; j < numRuns; ++j) {	
		    char *inputBufferCopy = malloc(inputSize);
		    memcpy(inputBufferCopy, inputBuffer, inputSize);
		    outputBuffer = malloc(NUM_COORDINATES * MAX_OUTPUT_LINE_SIZE *
            sizeof(char));
            int runs = i + 1;
			gettimeofday(&startTime, NULL);
			convert_to_csv(inputBufferCopy, &outputBuffer, runs);
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
	write_doubles_to_csv_file(timingArray, numImages, TIMING_FILEPATH_CLOUD);
    
    free(inputBuffer);
}

