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

#define TIME_OUTPUT	"./timing.txt"

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

int write_json_title (char *file_buf, char *title, int *buf_pos)
{
	int len;
	
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

int write_num_json (int fd, int is_string, int nest_level, char *title, 
			double value, int *first)
{
	char file_buf[512];
	int buf_len = 0;
	int len;
	int buf_pos = 0;
	
	nest_level *= 4;
	
	if (fd < 0 || nest_level < 0 || nest_level > sizeof(file_buf) || !title)
		return -1;
	
	memset(file_buf, 0, sizeof(file_buf));
	
	for (buf_pos = 0; buf_pos < nest_level; buf_pos++)
		file_buf[buf_pos] = ' ';

	write_json_title(file_buf, title, &buf_pos);
	 
	len = snprintf(file_buf + buf_pos, sizeof(file_buf) - buf_pos, "%lf", value);
		
	buf_pos += len;
	
	if (*first) {
		*first = 0;
	} else {
		if (write(fd, ",\n", 2) != 2)
			return -1;
	}

	buf_len = (int)strlen(file_buf);
	
	return write(fd, file_buf, buf_len) == buf_len ? 0 : -1;
}	


/* TO DO: add support for arrays */
/* only for use with .fits tag reader function */
int write_tag_json (char *file_buf, int is_string, int nest_level, 
			char *title, char *text, int *first)
{
	int i, len, buf_pos = 0;
	char tmp;
	double val;
	
	nest_level *= 4;
	
	if (!title || !text || !first || !file_buf)
		return -1;
		
	if (*first) {
		*first = 0;
	} else {
		strncpy(file_buf + buf_pos, ",\n", 2);
		buf_pos += 2;
	}
	
	while (nest_level--)
		file_buf[buf_pos++] = ' ';
		
	if (write_json_title(file_buf, title, &buf_pos) < 0)
		return -1;
	 
	sscanf(text, "%*[ ]%c", (char *)&tmp);
	
	if (is_string && (len = (int)strlen(text)) == 0) {
		strncpy(file_buf + buf_pos, "null", 4);
		buf_pos += 4;
	} else if (is_string) {
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
	} else if (tmp == 'T') {
		strncpy(file_buf + buf_pos, "true", 4);
		buf_pos += 4;
	} else if (tmp == 'F') {
		strncpy(file_buf + buf_pos, "false", 5);
		buf_pos += 5;
	} else {
		sscanf(text, "%*[ ]%lf", &val);
		len = sprintf(file_buf + buf_pos, "%lf", val);
		
		buf_pos += len;
	}

	return buf_pos;
}	




int find_lowest_x_floats (float *src, int num, float **dst, int k)
{
	int i, o_pos = 0, mi;
	
	if (k >= num || !src || !dst)
		return -1;
	
	*dst = calloc(sizeof(float), k);
	
	for (i = 0; i < k; i++) {
		mi = find_min_index(src, num);
		(*dst)[o_pos++] = src[mi];
		src[mi] = FLT_MAX;
	}

	return 0;
}

/* writes timing data as .json, "k" is the number of quickest runs to save */
int write_time_file (float *time_array, int runs, int k)
{ 
	int first = 1;
	int time_fd, i;
	float *lowest_times;
	double avg_lowest = 0;
	struct utsname un;
	char tmp_buf[512];
	int out_len;
	time_t ts_val;
	char time_buf[80];
	
	if ((time_fd = open(TIME_OUTPUT, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0)//0777 mode allows all (user,group,others) to read write and execute
		{
			printf("time_fd\n");
			return -1;
		}

	if (write(time_fd, "{\n", 2) != 2 ||
		write_num_json(time_fd, 0, 1, "total # runs", (double)runs, &first) < 0 ||
		write_num_json(time_fd, 0, 1, "# runs saved", (double)k, &first) < 0)
		{
			printf("write\n");
			return -1;		
		}	
	find_lowest_x_floats(time_array, runs, &lowest_times, k);
	
	for (i = 0; i < k; i++) {
		if (write_num_json(time_fd, 0, 1, "microseconds", 
				   (double)lowest_times[i], &first) < 0)
			{
				printf("lowest_times\n");
			return -1;
			}
		avg_lowest += (lowest_times[i] / k);
	}

	free(lowest_times);


// Writing System Information
	
	if (uname(&un))
		printf("error getting OS data\n");

	if ((out_len = write_tag_json (tmp_buf, 1, 1, "sysname", un.sysname, &first)) > 0)
		if (write(time_fd, tmp_buf, out_len) != out_len)
			printf("error writing tag\n");
			
	if ((out_len = write_tag_json (tmp_buf, 1, 1, "release", un.release, &first)) > 0)
		if (write(time_fd, tmp_buf, out_len) != out_len)
			printf("error writing tag\n");
			
	if ((out_len = write_tag_json (tmp_buf, 1, 1, "machine", un.machine, &first)) > 0)
		if (write(time_fd, tmp_buf, out_len) != out_len)
			printf("error writing tag\n");		

// Writing Time

	time(&ts_val);
	strftime(time_buf, 80, "%x - %I:%M%p", localtime(&ts_val));
	
	if ((out_len = write_tag_json (tmp_buf, 1, 1, "time", time_buf, &first)) > 0)
		if (write(time_fd, tmp_buf, out_len) != out_len)
			printf("error writing tag\n");	
	
	if (write_num_json(time_fd, 0, 1, "avg. microseconds", avg_lowest, &first) < 0||
		write(time_fd, "\n}", 2) != 2)
		{
			printf("avg_lowest\n");
			return -1;
		}

	close(time_fd);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
}

/* research compression methods and find ease of decoding,  */
/* find larger test images */

/*void f32_to_u8_scaled (float *big_endian, unsigned char *dst, int num)
{
	int i, x;
	double sum2 = 0, sum3 = 0, mean = 0;
	double variance2, ratio, tmp;
	float min, max;
	
	float *src = calloc(sizeof(float), num);
	
	memcpy(src, big_endian, sizeof(float) * num);
	
	for (x = 0; x < num; x++) {
		char *ptr = (char *)&src[x];
		
		swap(&ptr[0], &ptr[3]); 
		swap(&ptr[1], &ptr[2]);
	}

	min = src[find_min_index(src, num)];
	max = src[find_max_index(src, num)];

	for (x = 0; x < num; x++)
		mean += (double)((double)src[x] / (double)num);	
	
	for (x = 0; x < num; x++) {
		tmp = ((double)src[x] - mean);
		sum2 += tmp * tmp;
		sum3 += tmp;
	}
	
	variance2 = (sum2 - ((sum3 * sum3) / (double)num)) / ((double)num);
	
	fprintf(stderr, "[%s]: range = (%f -> %f), mean = %lf, variance = %lf\n",
		__func__, min, max, mean, variance2);

	ratio = log(max) / 255.0f; 

	for (i = 0; i < num; i++) {
		dst[i] = (log(src[i]) / ratio);
		
		//printf("src = %lf, ratio=%lf min = %f, max = %f, dst = %d log=%lf\n", 
			//src[i], ratio, min, max, dst[i], log(src[i]) / ratio);
	}
	
	free(src);
}

int main (void)
{
	struct fits_convert fts;
	
	memset(&fts, 0, sizeof(struct fits_convert));
	
	fts.path_input = "/Users/nobody1/Desktop/green.fits";
	fts.path_tiff = "/Users/nobody1/Desktop/test.tiff";
	fts.path_metadata = "/Users/nobody1/Desktop/test.json";
	fts.path_timedata = "/Users/nobody1/Desktop/time.json";

	unsigned char *buf; 
	int file_size, i;
	uint64_t usec;
	int k = 8, runs = 20;
	float *time_array = calloc(runs, sizeof(float));
	
	if ((file_size = (int)read_file(fts.path_input, &buf)) < 0) {
		printf("could not read file\n");
		return -1;
	}

	for (i = 0; i < runs; i++) {
		free(fts.img_out);
		free(fts.metadata);
		usec = run_conversion(buf, file_size, &fts, &fts.img_out, 
					&fts.img_size, &fts.metadata, &fts.metadata_size);
		time_array[i] = (float)usec;
	}
	
	if (write_time_file(time_array, runs, k) < 0)
		printf("error writing time file\n");

	if (create_file_with_data(fts.path_tiff, fts.img_out, fts.img_size) < 0)
		printf("error writing tiff file\n");
	
	if (create_file_with_data(fts.path_metadata, 
				(unsigned char *)fts.metadata, fts.metadata_size) < 0)
		printf("error writing json file\n");
	
	free(fts.metadata);
	free(fts.img_out);
	
	free(time_array);
	free(buf);
	
	return 0;
}
*/	

