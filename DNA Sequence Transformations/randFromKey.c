/*
*randDivAndOptFile.c
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int keyIdentifier(FILE * ifp,char *input,int *output){
	char byt; 
	int inputCount = 0; 
	int outputCount = 0;
	int i = 0, j;
	char numBuf[20];
	while((byt=getc(ifp))!= EOF){
	
		if(byt != ' ' && byt != '\n' && byt != '\r' && byt!= ','){
			if(byt != '>'){
				input[inputCount++] = byt;
			}else{
				while((byt = getc(ifp)) == ' ' || byt == '\n' || byt == '\r' || byt == ','){}
				i=0;
				while(byt != '\n'){
					 numBuf[i++] = byt;
					 byt=getc(ifp);
				}
				numBuf[i++] = '\x00';
				
			//	output[outputCount++] = atoi(numBuf);
			//	continue; 
				
				for(j=0; j < i-1; ++j){
					if(atoi(numBuf) == 0 && numBuf[j] != '0'){ // If it's a character, cast and put into output
						output[outputCount++] = (int)numBuf[j]; 
						break;
					}
					else{
						output[outputCount++] = atoi(numBuf); //If it's a number, convert to int and put into output
						break;
					}
				}
				
			}
		}
	}
	return inputCount;
}

int getFileSize(FILE *ifp){
 // Go to the end of the file //
 	int keySize;
	if(fseek(ifp, 0L, SEEK_END)== 0){
	// Get the size of the file. //
		keySize = ftell(ifp);
		if (keySize == -1) {
			fputs("Error finding size of file", stderr);
		}
		// Return to start of file //
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
		}
	}
	return keySize;
}

int randDivAndOpt (char* fileName, int fileSize, char* characters, int numChar){
	int i;
	time_t currentTime;
	srand((unsigned int)time(&currentTime)); 
	FILE * ofp;
	if((ofp = fopen(fileName,"w"))!= NULL){
		//char dateAndTime [80]; 			//Create Header Line
		//strftime(dateAndTime, 80, "%x - %I:%M%p", localtime(&currentTime));
		//fprintf(ofp,"%s %s %s %s\n","> Randomly Generated File on", dateAndTime, "File Name:",fileName);

		for(i=0; i < fileSize; ++i){
			int letter = rand()%numChar;		
			fprintf(ofp,"%c",characters[letter]);
			if(i%70 == 69){			// Newline every 70 characters (custom)
				fprintf(ofp,"%c",'\n');
			}
		}
		
		fclose(ofp);
		printf("\"%s\" %s\n",fileName,"divAndOptfile created.");
		return 0;
	}else {
		printf("Error writing file");	
		return -1;
	}

}


int main (int argc, char * argv[]){ //[key] [fileSize] [outputFile]

	if(argc==4){
		FILE *ifp;
		if((ifp = fopen(argv[1],"r")) != NULL){
			int keySize = getFileSize(ifp);
			char * input = malloc(keySize*sizeof(char));
			int * output = malloc(keySize*sizeof(int));
			keySize = keyIdentifier(ifp,input,output);
			randDivAndOpt(argv[3],atoi(argv[2]),input,keySize);
			return 0;
		} else {
			printf("key not accessible");
			return -1;
		}
	} else {
	printf("Invalid Arguments");
	return -1;
	}
}
