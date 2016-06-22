#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void array_test(unsigned char array[3][3]) {
	array[1][1] = 55;
}

int main(){
	unsigned char array[3][3] = {0};
	unsigned char c = (unsigned char)(256);
	printf("%d\n", c);
	array[0][0] = c;
	array_test(array);
	int i,j;
	for (i = 0; i < 3; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			printf("%d ", (int)array[i][j]);
		}
		printf("\n");
	}
}