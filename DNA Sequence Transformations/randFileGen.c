/*
*randFileGen.c
*
*Generates random DNA sequence files for testing
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int randGen (char* fileType, int fileSize, char* fileName) {
	int i;
	srand((unsigned int)time(NULL)); 
	if(!strcmp(fileType,"f")||!strcmp(fileType,"F")){ //FASTA
		FILE * ofp;
		if((ofp = fopen(fileName,"w"))!= NULL){
		for(i=0; i < fileSize; ++i){
			int letter = rand()%4;
			if(letter == 0){			
				fprintf(ofp,"%c",'A');
			}else if(letter == 1){			
				fprintf(ofp,"%c",'C');
			}else if(letter == 2){			
				fprintf(ofp,"%c",'G');
			}else if(letter == 3){			
				fprintf(ofp,"%c",'T');
			}
		}
		
		fclose(ofp);
		printf("\"%s\" %s\n",fileName,"FASTA file created.");
		}else {
			printf("Error writing File");	
		}
	}else if(!strcmp(fileType,"m")||!strcmp(fileType,"M")){ // Multi-FASTA
		printf("Sorry, this feature is currently unavailble.");
	}else if(!strcmp(fileType,"q")||!strcmp(fileType,"Q")){ // FASTQ
		printf("Sorry, this feature is currently unavailble.");
	}else if(!strcmp(fileType,"s")||!strcmp(fileType,"S")){ // SAM
		printf("Sorry, this feature is currently unavailble.");
	}else if(!strcmp(fileType,"b")||!strcmp(fileType,"B")){ // BAM
		printf("Sorry, this feature is currently unavailble.");
	}else{
		printf("invalid input type");
		return -1;
	}

	return 0; 

}

int main (int argc, char ** argv){ // [char fileType] [int fileSize (in bytes)] [FileName]

	if(argc == 4){
		randGen(argv[1], atoi(argv[2]), argv[3]);
		return 0;
	}
	else{
		printf("Invalid Arguments. Please try again.");
		return -1;
	}

}
