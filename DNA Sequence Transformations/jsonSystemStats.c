#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <time.h> 
#include "jsonData.h"


int main(int argc, char** argv){ //[fileName]   Used to print system information to data file
	// Writing System Information
	int first = 1; 
	int nest_level = 0;
	struct utsname un;
	char tmp_buf[512];
	int out_len;
	time_t ts_val;
	char time_buf[80];
	FILE * dfp;
	if(argc != 2){
		printf("bad arguments");
		return -1;
	}
	if((dfp = fopen(argv[1],"w")) == NULL)
		{
			printf("Data file could not be appended\n"); // File could not be created and opened
			return -1;
		}
		
	
	if (uname(&un))
		printf("error getting OS data\n");


	if ((out_len = write_tag_json (dfp, nest_level, "sysname", un.sysname, &first)) < 0)
	
			printf("error writing tag\n");
			
	if ((out_len = write_tag_json (dfp, nest_level, "release", un.release, &first)) < 0)
			printf("error writing tag\n");
			
	if ((out_len = write_tag_json (dfp, nest_level, "machine", un.machine, &first)) < 0)
			printf("error writing tag\n");
			
			
	// Writing Time

	time(&ts_val);
	strftime(time_buf, 80, "%x - %I:%M%p", localtime(&ts_val));
	
	if ((out_len = write_tag_json (dfp, nest_level, "time", time_buf, &first)) < 0)
			printf("error writing tag\n");
			
	fprintf(dfp,"%c\n",',');
	
	fclose(dfp);
	return 0;
}


