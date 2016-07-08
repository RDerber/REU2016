/*
 * writeJson.c
 * Modified to give a general timing report for DNA File Transformation Tests
 */

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
#include "writeJson.h"

#define TEMP_FILE	"./temp.json"

/* read data from json file */
/* document code */


/****************************************************************************************/
/*	util functions */
/****************************************************************************************/


void swap (char *a, char *b)
{
	char c = *a;
	
	*a = *b;
	*b = c;
}

int find_max_index (float *src, int num)
{
	int max = 0;

	while (num--)
		if (src[num] > src[max])
			max = num;
	
	return max; 
}

int find_min_index (float *src, int num)
{
	int min = 0;

	while (num--)
		if (src[num] < src[min])
			min = num;
	
	return min; 
}


/***************************************************************************************************************************
*			write_json Methods
****************************************************************************************************************************/

int write_json_label (char *file_buf, char *title, int *buf_pos, int nest_level)
{
	int len;
	
	nest_level *= 4;
	
	for (buf_pos = 0; buf_pos < nest_level; buf_pos++)
		file_buf[buf_pos] = ' ';
	
	file_buf[(*buf_pos)++] = '\"';
	if ((len = (int)strlen(title)) == 0)
		return -1;
	
	strncpy(file_buf + (*buf_pos), title, len);
	(*buf_pos) += len;
	 
	file_buf[(*buf_pos)++] = '\"';
	file_buf[(*buf_pos)++] = ':';
	file_buf[(*buf_pos)++] = ' ';
	
	return 0;
}

int write_json_title (char *file_buf, char *title, int *buf_pos)
{
	int len;
	nest_level *= 4;
	
	for (buf_pos = 0; buf_pos < nest_level; buf_pos++)
		file_buf[buf_pos] = ' ';
	
	file_buf[(*buf_pos)++] = '\"';
	if ((len = (int)strlen(title)) == 0)
		return -1;
	
	strncpy(file_buf + (*buf_pos), title, len);
	(*buf_pos) += len;
	 
	file_buf[(*buf_pos)++] = '\"';
	file_buf[(*buf_pos)++] = ':';
	file_buf[(*buf_pos)++] = ' ';
	file_buf[(*buf_pos)++] == '{';
	file_buf[(*buf_pos)++] == '\n';
	
	return 0;
}


int write_num_json (FILE * dfp, int nest_level, char *title, 
			double value, int *first)
{
	char file_buf[512];
	int buf_len = 0;
	int len;
	int buf_pos = 0;
	
	if (dfp == NULL || nest_level < 0 || nest_level > sizeof(file_buf) || !title)
		return -1;
	
	memset(file_buf, 0, sizeof(file_buf));


	write_json_label(file_buf, title, &buf_pos, nest_level);
	 
	len = snprintf(file_buf + buf_pos, sizeof(file_buf) - buf_pos, "%lf", value);
		
	buf_pos += len;
	
	if (*first) {
		*first = 0;
	} else {
		if (fprintf(dfp,"%s", ",\n") != 2)
			return -1;
	}

	buf_len = (int)strlen(file_buf);
	
	return fwrite(file_buf, sizeof(char), buf_len, dfp) == buf_len ? 0 : -1;
}	


/* TO DO: add support for arrays */
/* only for use with .fits tag reader function */
int write_tag_json (char *file_buf, int nest_level, 
			char *title, char *text, int *first)
{
	int i, len, buf_pos = 0;
	char tmp;
	double val;
	
	if (!title || !text || !first || !file_buf)
		return -1;
		
	if (*first) {
		*first = 0;
	} else {
		strncpy(file_buf + buf_pos, ",\n", 2);
		buf_pos += 2;
	}
	
		
	if (write_json_label(file_buf, title, &buf_pos) < 0)
		return -1;
	 
	sscanf(text, "%*[ ]%c", (char *)&tmp);
	
	if ((len = (int)strlen(text)) == 0) {
		strncpy(file_buf + buf_pos, "null", 4);
		buf_pos += 4;
	}else {
		file_buf[buf_pos++] = '\"';
		
		/* remove trailing spaces */
		while (text[len - 1] == ' ' && len)	
			--len;
		 
		for (i = 0; i < len; i++) {
			if (text[i] == '\\' || text[i] == '\"')
				file_buf[buf_pos++] = '\\';
			file_buf[buf_pos++] = text[i];
		}
		file_buf[buf_pos++] = '\"';
	}	
	

	return buf_pos;
}

int write_super_data_file (double **data_arr, char **label_arr, long num_labels, long num_runs, char * opsSeq, int * numSeq, int maxNumOps, char* input, char* output, int inputSize)
{
	int first = 1;
	int nest_level = 0;
	int i, j;

	FILE * dfp;
	
	
	int numOps;
	for(i=0; i<maxNumOps; ++i){
		if(numSeq == 0) ++numOps;
	}
	
//	if ((data_fd = open(TEMP_FILE, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0)//0777 mode allows all (user,group,others) to read write and execute
	if((dfp = fopen(TEMP_FILE,"a")) == NULL)
		{
			printf("Data file could not be appended\n"); // File could not be created and opened
			return -1;
		}
		
//	char title_buf[100];					//Write out file title and incease nest level 
//	int title_size = 0;
//	if(write_json_title(title_buf, title , &title_size) <0){
//		printf("Error writing title");
//		return -1;
//	}
	
//	fwrite(title_buf, sizeof(char), title_size. dfp);
//	++nest_level; 

	
	//print timing data
	for(i = 0; i < num_labels; ++i){
		char nest_spaces [nest_level*4];
		memset (nest_spaces, ' ', sizeof(nest_spaces));
		fwrite(nest_spaces, sizeof(char),(nest_level*4),dfp); 		// indent 4 spaces for each nest level	
		char label_buf[80];					//Write label title before printing data values
		int label_size = 0;
		if(write_json_title(label_buf, label_arr[i],&label_size)<0){
			printf("Error writing label");
			return -1;
		}
		++nest_level;
		fwrite(label_buf, sizeof(char), label_size, dfp);
		for(j = 0; j < num_runs; ++j){ // print data values for each run
			char run_title [10];
			sprintf(run_title, "Run %d", j);
			if(write_num_json(dfp, nest_level, run_title, data_arr[i][j].data, &first) < 0){ 
				printf("%s %d\n%s %s\n %s %d\n","error writing element: ", i, "label: ",
					data_array[i].label, "data: ", data_array[i].data);
				return -1;
			}
		}
		--nest_level; 
	}
	

	
			
	fprintf(dfp, "$c", '}'); 
	
	fclose(dfp);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
}

int main(int argc, char** argv){ [fileName]
	// Writing System Information
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
	if((dfp = fopen(argv[1],"a")) == NULL)
		{
			printf("Data file could not be appended\n"); // File could not be created and opened
			return -1;
		}
		
	
	if (uname(&un))
		printf("error getting OS data\n");


	if ((out_len = write_tag_json (tmp_buf, nest_level, "sysname", un.sysname, &first)) > 0)
		if (fwrite(tmp_buf, sizeof(char),out_len, dfp) != out_len)
			printf("error writing tag\n");
			
	if ((out_len = write_tag_json (tmp_buf, nest_level, "release", un.release, &first)) > 0)
		if (fwrite(tmp_buf, sizeof(char), out_len, dfp) != out_len)
			printf("error writing tag\n");
			
	if ((out_len = write_tag_json (tmp_buf, nest_level, "machine", un.machine, &first)) > 0)
		if (fwrite(tmp_buf, sizeof(char), out_len, dfp) != out_len)
			printf("error writing tag\n");
			
			
	// Writing Time

	time(&ts_val);
	strftime(time_buf, 80, "%x - %I:%M%p", localtime(&ts_val));
	
	if ((out_len = write_tag_json (tmp_buf, nest_level, "time", time_buf, &first)) > 0)
		if (fwrite(tmp_buf, sizeof(char), out_len, dfp) != out_len)
			printf("error writing tag\n");
	
	return 0;
}


