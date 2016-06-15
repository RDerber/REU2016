#include <stdio.h>
#include <stdlib.h>
/*Checks to see if 2 files are the same other than new lines, carriage returns, and spaces
*/
int main(int argc,char * argv[]){
	FILE * f1;
	FILE * f2;
	f1 = fopen(argv[1], "r");
	f2 = fopen(argv[2], "r");
	while(1){
		char check1;
		char check2;
		while((check1 = getc(f1)) == (check2 = getc(f2)) && check1 != EOF && check2 != EOF){
//			printf("%c", check1);
//			printf("%c", check2);
		}
		if(check1 == '\n' || check1 == ' ' || check1 == '\r'){
			ungetc(check2, f2);
		}else if(check2 == '\n' || check2 == ' ' || check2 == '\r'){
			ungetc(check1, f1);
		}else if(check1 == check2){
			printf("%s", "file contents are the same");
		}else{
			printf("%c", check1);
			printf("%c", '\n');
			printf("%c", check2);
			printf("%c", '\n');
			printf("%s","file contents differ");
			printf("%c", '\n');
			fclose(f1);
			fclose(f2);
			return 1;
		}
	}
}
