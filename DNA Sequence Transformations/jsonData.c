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
#include "jsonData.h"

#define TEMP_FILE	"./temp1.json"
#define TIME_OUT	"./timing.json"

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
	
	int nest_identation = nest_level + *buf_pos; 
	
	for ((*buf_pos); (*buf_pos) < nest_identation; (*buf_pos)++)
		file_buf[*buf_pos] = ' ';
	
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

int write_json_title (FILE * dfp, int nest_level, char *title, int *first)
{
	int len;
	nest_level *= 4;
	char file_buf[512];
	int buf_len = 0;
	int buf_pos = 0;
	
	if (*first) {
		*first = 0;
	} else {
		if (fprintf(dfp,"%s", ",\n") != 2)
			return -1;
	}
	
	for (buf_pos = 0; buf_pos < nest_level; buf_pos++)
		file_buf[buf_pos] = ' ';
	
	file_buf[buf_pos++] = '\"';
	if ((len = (int)strlen(title)) == 0)
		return -1;
	
	strncpy(file_buf + buf_pos, title, len);
	buf_pos += (len);
	 
	file_buf[buf_pos++] = '\"';
	file_buf[buf_pos++] = ':';
	file_buf[buf_pos++] = ' ';
	file_buf[buf_pos++] = '{';
	file_buf[buf_pos++] = '\n';
	
	//buf_len = (int)strlen(file_buf);
	
	return fwrite(file_buf, sizeof(char), buf_pos, dfp) == buf_pos ? 0 : -1;
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
int write_tag_json (FILE * dfp, int nest_level, 
			char *title, char *text, int *first)
{
	int i, len, buf_pos = 0;
	double val;
	char file_buf[512];
	int buf_len = 0;
	
	
	if (dfp == NULL || nest_level < 0 || nest_level > sizeof(file_buf) || !title)
		return -1;
	
	memset(file_buf, 0, sizeof(file_buf));
	
	if (!title || !text || !first || !file_buf)
		return -1;	
		
	if (*first) {
		*first = 0;
	} else {
		strncpy(file_buf + buf_pos, ",\n", 2);
		buf_pos += 2;
	}
		
	if (write_json_label(file_buf, title, &buf_pos, nest_level) < 0)
		return -1;
	
	
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
	
	buf_len = (int)strlen(file_buf);

	return fwrite(file_buf, sizeof(char), buf_len, dfp) == buf_len ? 0 : -1;
}

int write_super_file (double **data_arr, char **label_arr, int num_labels, int num_runs, char * opsSeq, int * numSeq, int maxNumOps, char* input, char* output, int inputSize){

/******************************************************************************************
input set 1:{ write_super_file output is formated as show below:

		input1: 1
		input2: 3
		.
		.
		.
		output1: 5 
		output2: 6
		.
		.
		.
		operation1: +3,
		operation2: &4,
		.
		.
		.
		numOps: 2
		runTime:{
			run1: data
			run2
			run3
			.
			.
			.
			}
		evalTime: {
			run1
			run2
			run3
			.
			.
			.
		} 
	}

******************************************************************************************/

	int first = 1; // First line has not been printed 
	int nest_level = 0; //Two titles come before the data is printed
	int i, j;
	FILE * dfp;
	int numOps = 0;
	for(i=0; i<maxNumOps; ++i){
		if(numSeq[i] != 0) ++numOps;
	}
	
	if((dfp = fopen(TEMP_FILE,"w+")) == NULL){
			printf("Data file could not be appended\n"); // File could not be created and opened
			return -1;
		}
		
	//print inputs
	for(i=0; i<inputSize; ++i){
		char inputLabel[12];
		snprintf(inputLabel,12,"input %d", i);
		write_num_json (dfp, nest_level, inputLabel, input[i], &first);
		//fprintf(dfp,"%s%d%s%d\n","input",i,": ", input[i]);
	}
	
	//print outputs
	for(i=0; i<inputSize; ++i){
		char outputLabel[12];
		snprintf(outputLabel,12 ,"output %d", i);
		write_num_json (dfp, nest_level, outputLabel, output[i], &first);
	}
	
	//print numOps
	
	write_num_json(dfp, nest_level, "numOps", numOps, &first);
	
	//print operations
	for(i=0; i<numOps; ++i){
		char opBuf [30];
		snprintf(opBuf,30 ,"operation %d", i);
		char opStr[2];
		opStr[0] = opsSeq[maxNumOps-numOps+i];
		opStr[1] = '\x00';
		write_tag_json (dfp, nest_level, opBuf, opStr, &first);
				
		char numBuf[30];
		snprintf(numBuf,30,"opvalue %d", i);
		write_num_json (dfp, nest_level, numBuf, numSeq[maxNumOps-numOps+i], &first);
			

		//fprintf(dfp,"%s%d%s %c%d\n","operation",i,": ", opsSeq[i], numSeq[i]);
	}
	
	
	//print timing data
	
	//write label title before printing data values
	for(i = 0; i < num_labels; ++i){
		//char label_buf[80];		
		//int label_size = 0;
		if(write_json_title(dfp, nest_level, label_arr[i], &first)<0){
			printf("Error writing label");
			return -1;
		}
		fflush(dfp);
		++nest_level;
		//fwrite(label_buf, sizeof(char), label_size, dfp);
		// print data values for each run
		first = 1;
		for(j = 0; j < num_runs; ++j){ 						
			char run_title [10];
			sprintf(run_title, "Run %d", j);
			if(write_num_json(dfp, nest_level, run_title, data_arr[i][j], &first) < 0){ 
				printf("%s %d\n%s %s\n %s %d\n","error writing element: ", i, "label: ", 
					label_arr[i], "data: ", data_arr[i]);
				return -1;
			}
		}
		--nest_level; 
		//Print the closing bracket one nest level lower than data
		fprintf(dfp, "%c", '\n'); 
		for(j=0; j<nest_level*4; ++j)
			fprintf(dfp, "%c", ' '); 
			
		fprintf(dfp, "%c", '}'); 
		
	}
	
	//Nest and print closing bracket for all data in input set
//	for(j=0; j<nest_level; ++j)
//		fprintf(dfp, "%c", ' '); 
			
//	fprintf(dfp, "%c", '}'); 
	
	
	fclose(dfp);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
}

int write_time_file (double **data_arr, char **label_arr, int num_labels, int num_runs){

/******************************************************************************************
write_timing_file output is formated as show below:
	{
		Transformation Time: {
			run1: data
			run2
			run3
			.
			.
			.
			}
		
	 
	}

******************************************************************************************/

	int first = 1; // First line has not been printed 
	int nest_level = 0; //Two titles come before the data is printed
	int i, j;
	FILE * dfp;
	
	if((dfp = fopen(TIME_OUT,"w+")) == NULL){
			printf("Data file could not be appended\n"); // File could not be created and opened
			return -1;
		}
		
	//print timing data
	
	//write label title before printing data values
	
	for(i = 0; i < num_labels; ++i){
		if(write_json_title(dfp, nest_level, label_arr[i], &first)<0){
			printf("Error writing label");
			return -1;
		}
		fflush(dfp);
		++nest_level;
		// print data values for each run
		first = 1;
		for(j = 0; j < num_runs; ++j){ 						
			char run_title [10];
			sprintf(run_title, "Run %d", j);
			if(write_num_json(dfp, nest_level, run_title, data_arr[i][j], &first) < 0){ 
				printf("%s %d\n%s %s\n %s %d\n","error writing element: ", i, "label: ", 
					label_arr[i], "data: ", data_arr[i]);
				return -1;
			}
		}
		--nest_level; 
		//Print the closing bracket one nest level lower than data
		fprintf(dfp, "%c", '\n'); 
		for(j=0; j<nest_level*4; ++j)
			fprintf(dfp, "%c", ' '); 
			
		fprintf(dfp, "%c", '}'); 
		
	}
		
	
	fclose(dfp);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
}

int write_divAndOpt_file (double **data_arr, char **label_arr, int num_labels, int num_runs, int keySize){

/******************************************************************************************
write_divAndOpt_file output is formated as show below:
	{	keySize: 3
		Transformation Time: {
			run1: data
			run2
			run3
			.
			.
			.
			}
		
	 
	}

******************************************************************************************/

	int first = 1; // First line has not been printed 
	int nest_level = 0; //Two titles come before the data is printed
	int i, j;
	FILE * dfp;
	
	if((dfp = fopen(TIME_OUT,"w+")) == NULL){
			printf("Data file could not be appended\n"); // File could not be created and opened
			return -1;
		}
		
	//print timing data
	
	//write label title before printing data values
	if(write_num_json(dfp, nest_level, "Key Size", keySize, &first) < 0){ 
		printf("%s %d","error writing key size: ", keySize);
		return -1;
	}
	for(i = 0; i < num_labels; ++i){
		if(write_json_title(dfp, nest_level, label_arr[i], &first)<0){
			printf("Error writing label");
			return -1;
		}
		fflush(dfp);
		++nest_level;
		// print data values for each run
		first = 1;
		for(j = 0; j < num_runs; ++j){ 						
			char run_title [10];
			sprintf(run_title, "Run %d", j);
			if(write_num_json(dfp, nest_level, run_title, data_arr[i][j], &first) < 0){ 
				printf("%s %d\n%s %s\n %s %d\n","error writing element: ", i, "label: ", 
					label_arr[i], "data: ", data_arr[i]);
				return -1;
			}
		}
		--nest_level; 
		//Print the closing bracket one nest level lower than data
		fprintf(dfp, "%c", '\n'); 
		for(j=0; j<nest_level*4; ++j)
			fprintf(dfp, "%c", ' '); 
			
		fprintf(dfp, "%c", '}'); 
		
	}
		
	
	fclose(dfp);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
}



