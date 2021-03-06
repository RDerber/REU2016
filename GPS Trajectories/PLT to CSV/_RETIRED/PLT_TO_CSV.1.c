#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ID "000"
#define LINE_OFFSET 6
#define MAX_LINE_SIZE 64

size_t load_plt_to_buffer(char *filepath, char **buffer) {
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

int count_GPS_coordinates(char *fileBuffer) {
    char *token;
    char *fileDelimiters = "\n";
    token = strtok(fileBuffer, fileDelimiters);
    int i;
    for (i = 0; i < LINE_OFFSET; ++i) {
        fileBuffer += strlen(token) + 1;
        token = strtok(fileBuffer, fileDelimiters);
    }
    int numCoordinates = 0;
    while (token != NULL) {
        fileBuffer += strlen(token) + 1;
        token = strtok(fileBuffer, fileDelimiters);
        numCoordinates++;
    }
    return numCoordinates;
}

int input_coordinates(char *token, int numCoordinates, 
        double csvCoordinates[numCoordinates][2], int currIndex, 
        char **marker) {
    char *tokenDelimiters = ",";
    char *lattitude;
    char *longitude;
    char *comma = ",";
    char *newline = "\n";
    
    lattitude = strtok(token, tokenDelimiters);
    longitude = strtok(NULL, tokenDelimiters);
    
    printf("lattitude: %s\n", lattitude);
    printf("longitude: %s\n", longitude);
    
    // fprintf(*marker,"%s,%s\n", lattitude, longitude);
    memcpy(*marker, ID, strlen(ID) * sizeof(char));
    *marker += strlen(ID) * sizeof(char);
    memcpy(*marker, comma, sizeof(char));
    *marker += sizeof(char);
    memcpy(*marker, lattitude, strlen(lattitude) * sizeof(char));
    *marker += strlen(lattitude) * sizeof(char);
    memcpy(*marker, comma, sizeof(char));
    *marker += sizeof(char);
    memcpy(*marker, longitude, strlen(longitude) * sizeof(char));
    *marker += strlen(longitude) * sizeof(char);
    memcpy(*marker, newline, 1);
    *marker+= sizeof(char);
    
    // double lattitude, longitude;
    // lattitude = atof(strtok(token, tokenDelimiters));
    // longitude = atof(strtok(NULL, tokenDelimiters));
    // csvCoordinates[currIndex][0] = lattitude;
    // csvCoordinates[currIndex][1] = longitude;
    // printf("%f\n", lattitude);
    // printf("%f\n", longitude);
    
    return (strlen(ID) + strlen(lattitude) + strlen(longitude) + 3) * sizeof(char);
}



int convert_to_csv(char *fileBuffer, int numCoordinates, char **csvBuffer) {
    double csvCoordinates[numCoordinates][2];
    char *fileDelimiters = "\n";
    char *token = malloc(MAX_LINE_SIZE * sizeof(char));

    token = strtok(fileBuffer, fileDelimiters);
    char *marker = *csvBuffer;
    
    int i;
    for (i = 0; i < LINE_OFFSET; ++i) {
        fileBuffer += strlen(token) + 1;
        token = strtok(fileBuffer, fileDelimiters);
    }
    
    int fileSize = 0;
    i = 0;
    while (token != NULL) {
        fileBuffer += strlen(token) + 1;
        // printf("%s\n", token);
        fileSize += input_coordinates(token, numCoordinates, csvCoordinates, i, 
                &marker);
        // printf("%f\n", csvCoordinates[i][0]);
        // printf("%f\n", csvCoordinates[i][1]);
        i++;
        token = strtok(fileBuffer, fileDelimiters);
    }
    free(token);
    return fileSize;
}

void write_csv_file(char *csvBuffer, int fileSize) {
    FILE *file = fopen("test.csv", "w");
    fwrite(csvBuffer, fileSize, sizeof(char), file);
    fclose(file);
}

int main() {
    char *fileBuffer;
    char *csvBuffer;
    
    size_t fileLength = load_plt_to_buffer("000/20081023025304.plt", &fileBuffer);
    // printf("%s", fileBuffer);
    int numCoordinates = count_GPS_coordinates(fileBuffer);
    
    csvBuffer = malloc(numCoordinates * MAX_LINE_SIZE * sizeof(char));
    
    int fileSize = convert_to_csv(fileBuffer, numCoordinates, &csvBuffer);
    
    write_csv_file(csvBuffer, fileSize);
    
    printf("%s", csvBuffer);
    
    free(fileBuffer);
    free(csvBuffer);
    
    return 0;

}