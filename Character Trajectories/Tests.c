#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int main(){
  char string[80] = "SEGMENT DIGIT    0 ? \"8\"";
  int number = atoi(strtok(string, "\""));
  number = atoi(strtok(NULL, "\""));
  printf("%d\n", number);

	
}