/*
*randFileGen.c
*
*Generates random DNA sequence files for testing
*
* Input: [char fileType] [int fileSize (in bytes)] [name of file to be created]
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int randGen (char* fileType, int fileSize, char* fileName, int numSeq) {
	int i;
	time_t currentTime;
	srand((unsigned int)time(&currentTime)); 
	if(!strcmp(fileType,"f")||!strcmp(fileType,"F")){ //FASTA
		FILE * ofp;
		if((ofp = fopen(fileName,"w"))!= NULL){
		char dateAndTime [80]; 			//Create Header Line
		strftime(dateAndTime, 80, "%x - %I:%M%p", localtime(&currentTime));
		fprintf(ofp,"%s %s %s %s\n","> Randomly Generated File on", dateAndTime, "File Name:",fileName);
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
			if(i%70 == 69){			// Newline every 70 characters (custom)
				fprintf(ofp,"%c",'\n');
			}
		}
		
		fclose(ofp);
		printf("\"%s\" %s\n",fileName,"FASTA file created.");
		}else {
			printf("Error writing file");	
		}
	}else if(!strcmp(fileType,"m")||!strcmp(fileType,"M")){ // Multi-FASTA
		int j;
		FILE * ofp;
		if((ofp = fopen(fileName,"w"))!= NULL){
			char dateAndTime [80]; 			//Create Header Line
			strftime(dateAndTime, 80, "%x - %I:%M%p", localtime(&currentTime));
			fprintf(ofp,"%s %s %s %s\n",">Randomly Generated File on", dateAndTime, "File Name:",fileName);		
			int seqSize = fileSize/numSeq;
			int remainder = fileSize%numSeq;
	
			for(i=0; i<numSeq; i++){
				if(i!= 0){
					fprintf(ofp,"\n%s %d\n",">Sequence Header #",(i+1));
				}
				if(i != numSeq-1){
					for(j=0; j < seqSize; ++j){
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
						if(j%70 == 69){			// Newline every 70 characters (custom)
							fprintf(ofp,"%c",'\n');
						}
					}
				}
				else{					// Attatch remainder to the last sequence 
					for(j=0; j < seqSize+remainder; ++j){
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
						if(j%70 == 69){			// Newline every 70 characters (custom)
							fprintf(ofp,"%c",'\n');
						}
					}
				}
			}
	
			fclose(ofp);
			printf("\"%s\" %s\n",fileName,"Multi-FASTA file created.");	
			
		}else{		
			printf("Error writing file");
		}
	}else if(!strcmp(fileType,"q")||!strcmp(fileType,"Q")){ // FASTQ
		printf("Sorry, this feature is currently unavailble.");
	}else if(!strcmp(fileType,"s")||!strcmp(fileType,"S")){ // SAM
		printf("Sorry, this feature is currently under contruction."); 
		return 0;

		FILE * ofp;
		if((ofp = fopen(fileName,"w"))!= NULL){
			char dateAndTime [80]; 			//Create Header Line
			strftime(dateAndTime, 80, "%x - %I:%M%p", localtime(&currentTime));
			fprintf(ofp,"%s %s %s %s\n","@Rand\t", dateAndTime, "FN:\t",fileName);	
			for(i=0; i<10; i++){
				fprintf(ofp,"%s\t%s%d\t%s%d \n","@SQ","SN:",(rand()%25),"LN:",rand());
			}	
			int seqSize = fileSize/numSeq;
			int remainder = fileSize%numSeq;
		
		fclose(ofp);
			printf("\"%s\" %s\n",fileName,"SAM file created.");	
			
		}else{		
			printf("Error writing file");
		}
		
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
		randGen(argv[1], atoi(argv[2]), argv[3], 1);
		return 0;
	}
	else if (argc == 5){
		randGen(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
		return 0;
	}
	else{
		printf("Invalid Arguments. Please try again.");
		return -1;
	}

}
