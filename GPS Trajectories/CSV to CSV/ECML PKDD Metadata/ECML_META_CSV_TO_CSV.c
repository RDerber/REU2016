/**
 * Transforms the ECML PKDD metadata csv file format into a generalized GPS 
 * trajectory csv file format of "ID,LATTITUDE,LONGITUDE". The input and output 
 * filepaths are defined as constants. All GPS coordinates within the input file 
 * will be included wihtin the exported generalized-format csv file.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_FILEPATH "metaData_taxistandsID_name_GPSlocation.csv"
#define OUTPUT_FILEPATH "metaData_taxistandsID_name_GPSlocation_transformed.csv"
#define LINE_OFFSET 1
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
    
    // Read in useful data 
    id = strtok(token, tokenDelimiters);
    strtok(NULL, tokenDelimiters);
    lattitude = strtok(NULL, tokenDelimiters);
    longitude = strtok(NULL, tokenDelimiters);
    
    printf("%s,%s,%s\n", id, lattitude, longitude);
    
    // Copy data in generalized format into output buffer
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
    *marker += sizeof(char);
    
    return (strlen(id) + strlen(lattitude) + strlen(longitude) + 3) * 
            sizeof(char);
}


// Converts the provided input file buffer into a generalized csv format and 
// writes it into the provided output buffer
// Returns the size of the output buffer
size_t convert_to_csv(char *inputBuffer, char **outputBuffer) {
    char *fileDelimiters = "\n";
    char *token = malloc(MAX_LINE_SIZE * sizeof(char));

    token = strtok(inputBuffer, fileDelimiters);
    char *marker = *outputBuffer;
    
    // Ignore headers/unimportant data
    int i;
    for (i = 0; i < LINE_OFFSET; ++i) {
        inputBuffer += strlen(token) + 1;
        token = strtok(inputBuffer, fileDelimiters);
    }
    
    size_t fileSize = 0;
    while (token != NULL) {
        inputBuffer += strlen(token) + 1;
        fileSize += input_coordinates(token, &marker);
        token = strtok(inputBuffer, fileDelimiters);
    }
    free(token);
    return fileSize;
}


// Writes file of provided file size with contents of provided buffer
void write_csv_file(char * filepath, char *buffer, size_t fileSize) {
    FILE *file = fopen(filepath, "w");
    fwrite(buffer, fileSize, sizeof(char), file);
    fclose(file);
}


void main() {
    char *inputBuffer;
    char *outputBuffer;
    
    size_t inputSize = load_csv_to_buffer(INPUT_FILEPATH, 
            &inputBuffer);

    int numCoordinates = count_GPS_coordinates(inputBuffer);
    
    outputBuffer = malloc(numCoordinates * MAX_LINE_SIZE * sizeof(char));
    
    // convert the csv file
    size_t outputSize = convert_to_csv(inputBuffer, &outputBuffer);
    
    write_csv_file(OUTPUT_FILEPATH, outputBuffer, 
            outputSize);
    
    printf("%s", outputBuffer);
    
    free(inputBuffer);
    free(outputBuffer);
}