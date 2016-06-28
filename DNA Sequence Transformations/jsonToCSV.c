/*
*jsonToCSV.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int jsonToCSV(FILE * ifp, FILE * ofp){
	char input;
	int i;
	int titleWritten = 0;
	int numComments = 2;

	while(!titleWritten){
		if((input=getc(ifp)) == 'm')
			if((input=getc(ifp) =='a'))
				if((input=getc(ifp) =='c'))
					if((input=getc(ifp) =='h')){
						fprintf(ofp,"%c",'#');
						fprintf(ofp,"%s", "mach");
						int numNewLine = 0;
						while(numNewLine < 2){
							if((input=getc(ifp))=='\n'){
								++numNewLine;
								if(numNewLine < numComments){
									while(getc(ifp) == ' '); // remove indentation spaces
										fprintf(ofp,"\n%c",'#');
								} else fprintf(ofp,"%c",'\n');
							}
							else fprintf(ofp,"%c",input);
						}
						titleWritten = 1; 
					}				
	}

	fseek(ifp, 0L, SEEK_SET);
	while((input=getc(ifp))!= EOF){
		
	
		if(input == 'f')
			if((input=getc(ifp) =='i'))
				if((input=getc(ifp) =='l'))
					if((input=getc(ifp) =='e')){
						while((input = getc(ifp)) != ':' || input == ' ');
						while((input=getc(ifp)) != ',')
							fprintf(ofp,"%c",input);
						fprintf(ofp, "%c", ',');				
						
					}

		if((input == 'a'))
			if((input=getc(ifp) =='v'))
				if((input=getc(ifp) =='g')){
						while((input = getc(ifp)) != ':' || input == ' ');
						while((input=getc(ifp)) != '\n')
							fprintf(ofp,"%c",input);
						fprintf(ofp, "%c",'\n');				
						
					}


				
	}


}

int main (int argc, char * argv[]){

	if(argc == 3){

		FILE * ifp = fopen(argv[1], "r");
		FILE * ofp = fopen(argv[2], "w");
		jsonToCSV(ifp,ofp);

		fclose(ifp);
		fclose(ofp);
		
		return 0;
	}else{
		printf("Invalid arguments");
		return -1; 
	}

}
