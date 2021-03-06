#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define LINE_OFFSET 1
#define MAX_LINE_SIZE 20000
#define NUM_FIELDS 9

size_t load_csv_to_buffer(char *filepath, char **buffer) {
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

// int count_GPS_coordinates(char *fileBuffer) {
//     char *fileDelimiters = "\n";
//     char *token;

//     token = strtok(fileBuffer, fileDelimiters);
    
//     int i;
//     for (i = 0; i < LINE_OFFSET; ++i) {
//         fileBuffer += strlen(token) + 1;
//         token = strtok(fileBuffer, fileDelimiters);
//     }
    
//     int fileSize = 0;
//     i = 0;
//     while (token != NULL) {
//         fileBuffer += strlen(token) + 1;
//         // printf("%s\n", token);
//         fileSize += helper_count_coordinates(token);
//         // printf("%f\n", csvCoordinates[i][0]);
//         // printf("%f\n", csvCoordinates[i][1]);
//         i++;
//         token = strtok(fileBuffer, fileDelimiters);
//     }
//     return fileSize;
// }

// int helper_count_coordinates(char *token) {
//     char *tokenDelimiters = ",[]";
//     char *lattitude;
//     char *longitude;
//     char *id;
//     char *comma = ",";
//     char *newline = "\n";
    
//     id = strtok(token, tokenDelimiters);
//     int i;
//     for (i = 0; i < NUM_FIELDS - 2; ++i) {
//         strtok(NULL, tokenDelimiters);   
//     }
//     lattitude = strtok(NULL, tokenDelimiters);
//     longitude = strtok(NULL, tokenDelimiters);
    
//     int length = 0;
//     while(lattitude != NULL && longitude != NULL) {
//         // printf("lattitude: %s\n", lattitude);
//         // printf("longitude: %s\n", longitude);
        
//         length += (strlen(id) + strlen(lattitude) + strlen(longitude) + 3) * 
//             sizeof(char);
        
//         lattitude = strtok(NULL, tokenDelimiters);
//         longitude = strtok(NULL, tokenDelimiters);
//     }
//     return length;
// }

long input_coordinates(char *token, int numCoordinates, 
        int currIndex, char **marker) {
    char *tokenDelimiters = ",[]\" ";
    char *lattitude;
    char *longitude;
    char *id;
    char *comma = ",";
    char *newline = "\n";
    
    id = strtok(token, tokenDelimiters);
    // printf("%s\n", id);
    int i;
    for (i = 0; i < NUM_FIELDS - 2; ++i) {
        strtok(NULL, tokenDelimiters);   
    }
    lattitude = strtok(NULL, tokenDelimiters);
    longitude = strtok(NULL, tokenDelimiters);
    // printf("reached4\n");
    long length = 0;
    while(lattitude != NULL && longitude != NULL) {
        // printf("reached5\n");
        // printf("lattitude: %s\n", lattitude);
        // printf("longitude: %s\n", longitude);
        
        // fprintf(marker,"%s,%s\n", lattitude, longitude);
        memcpy(*marker, id, strlen(id) * sizeof(char));
        *marker += strlen(id) * sizeof(char);
        memcpy(*marker, comma, sizeof(char));
        *marker += sizeof(char);
        memcpy(*marker, lattitude, strlen(lattitude) * sizeof(char));
        *marker += strlen(lattitude) * sizeof(char);
        memcpy(*marker, comma, sizeof(char));
        *marker += sizeof(char);
        memcpy(*marker, longitude, strlen(longitude) * sizeof(char));
        *marker += strlen(longitude) * sizeof(char);
        memcpy(*marker, newline, sizeof(char));
        *marker += sizeof(char);
        
        length += (strlen(id) + strlen(lattitude) + strlen(longitude) + 3) * 
            sizeof(char);
        
        lattitude = strtok(NULL, tokenDelimiters);
        if (lattitude != NULL) {
            longitude = strtok(NULL, tokenDelimiters);
        }
        // printf("reached6\n");
    }
    
    // double lattitude, longitude;
    // lattitude = atof(strtok(token, tokenDelimiters));
    // longitude = atof(strtok(NULL, tokenDelimiters));
    // csvCoordinates[currIndex][0] = lattitude;
    // csvCoordinates[currIndex][1] = longitude;
    // printf("%f\n", lattitude);
    // printf("%f\n", longitude);
    
    return length;
}



long convert_to_csv(char *fileBuffer, int numCoordinates, char **csvBuffer) {
    //double csvCoordinates[numCoordinates][2];
    char *fileDelimiters = "\n";
    char *token = malloc(MAX_LINE_SIZE * sizeof(char));
    
    // printf("reached2\n");

    token = strtok(fileBuffer, fileDelimiters);
    char *marker = *csvBuffer;
    
    int i;
    for (i = 0; i < LINE_OFFSET; ++i) {
        fileBuffer += strlen(token) + 1;
        token = strtok(fileBuffer, fileDelimiters);
    }
    
    long fileSize = 0;
    i = 0;
    while (token != NULL) {
        
        // printf("reached3\n");
        fileBuffer += strlen(token) + 1;
        // printf("%s\n", token);
        fileSize += input_coordinates(token, numCoordinates, i, 
                &marker);
        // printf("%f\n", csvCoordinates[i][0]);
        // printf("%f\n", csvCoordinates[i][1]);
        i++;
        token = strtok(fileBuffer, fileDelimiters);
    }
    free(token);
    return fileSize;
}

void write_csv_file(char *csvBuffer, long fileSize) {
    FILE *file = fopen("test.csv", "w");
    fwrite(csvBuffer, fileSize, sizeof(char), file);
    fclose(file);
}

int main() {
    char *fileBuffer;
    char *csvBuffer;
    
    size_t fileLength = load_csv_to_buffer(
            "Porto_taxi_data_test_partial_trajectories.csv", &fileBuffer);
    // printf("%s", fileBuffer);
    // int numCoordinates = count_GPS_coordinates(fileBuffer);
    // printf("Coordinates: %d\n", numCoordinates);
    int numCoordinates = 50;
    csvBuffer = malloc(14438 * 64 * sizeof(char));
    
    printf("reached1\n");
    
    long fileSize = convert_to_csv(fileBuffer, numCoordinates, &csvBuffer);
    printf("%ld", fileSize);
    
    
    write_csv_file(csvBuffer, fileSize);
    
    printf("%s", csvBuffer);
    
    return 0;
}