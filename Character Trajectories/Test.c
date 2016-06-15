#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

size_t WriteHexString(FILE *fptr, char *s, char *buffer, size_t space)
{
   unsigned int i,c;
   char hex[3];

   for (i=0;i<strlen(s);i+=2) {
      hex[0] = s[i];
      hex[1] = s[i+1];
      hex[2] = '\0';
      sscanf(hex,"%X",&c);
      buffer[space++] = (char)c;
   }

   return space;
}


int main(){
   FILE *ftpr;
   char *buffer;
   buffer = (char *)malloc(100 * sizeof(char));
   size_t space = 0;
   ftpr = fopen("test.txt", "wb");
   space = WriteHexString(ftpr, "010010110", buffer, space);
   fwrite(buffer, sizeof(char), space, ftpr);

   fclose(ftpr);


   unsigned char c = 0x19;
   unsigned int k = c;
   printf("%d\n", k);
   return 1;
}

