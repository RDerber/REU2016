/*
*randDivAndOptFile.c
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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


int main (int argc, char * argv[]){

if(argc>2){
	int i;
	int charSize = argc-3;
	char chars[charSize];
	for(i=3; i < argc; ++i){
		chars[i-3] = argv[i][0]; 
	}
	randDivAndOpt(argv[1],atoi(argv[2]), chars, charSize);
	
	return 0;
}
else{
	printf("Invalid Arguments");
	return -1; 
}

}
