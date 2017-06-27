#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <dirent.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h> 
#endif
#include "device_picker.h"
#include "err_code.h"

struct fits_convert {
	char *path_input;

	int x, y;
	int bitpix;
	int input_size;
	
	unsigned char *hist0, *hist1;
	int hs0, hs1;
	
	char *metadata;
	int metadata_size;
	
	unsigned char *img_out;
	int img_size;
};

/****************************************************************************************/
/*	tiff writer	*/
/****************************************************************************************/

int write_tiff_file (unsigned char **out, unsigned char *src, int x, int y)
{
	int total = (x * y), i;
	unsigned char *dst_ptr; 
	enum tiff_type { TYPE_BYTE = 1, TYPE_ASCII, TYPE_SHORT, TYPE_LONG, TYPE_RATIONAL };

#define TIFF_ENDIAN_LITTLE	0x4949
#define TIFF_ENDIAN_BIG		0x4D4D

#define TIFF_WIDTH 	0x0100
#define TIFF_HEIGHT	0x0101
#define BITS_PER_SAMPLE	0x0102	/* 4 or 8 for grayscale */
#define COMPRESS_METHOD	0x0103 	/* 1 for uncompressed */
#define PHTO_INTERPRET	0x0106 	/* 1 for grayscale (black=0)  */
#define STRIP_OFFSETS	0x0111
#define ROWS_PER_STRIP	0x0116	/* image height */
#define STRIP_BCOUNTS 	0x0117 	/* height * width * (3 for rgb) */
#define X_RESOLUTION	0x011a  /* rational number */
#define Y_RESOLUTION	0x011b	/* rational number */
#define RESOLUTION_UNIT	0x0153

	struct tiff_header {
		uint16_t endianness;
		uint16_t magic;
		uint32_t ifd_offset;
	} hdr;
	struct ifd_header {
		uint16_t tag_type;
		uint16_t field_type;
		uint32_t num_values;
		uint32_t value;	
	} hdrs[11] = {
		{TIFF_WIDTH, TYPE_LONG, 1, x}, 
		{TIFF_HEIGHT, TYPE_LONG, 1, y},
		{BITS_PER_SAMPLE, TYPE_SHORT, 1, 8},
		{COMPRESS_METHOD, TYPE_SHORT, 1, 1},
		{PHTO_INTERPRET, TYPE_SHORT, 1, 1},
		{STRIP_OFFSETS, TYPE_LONG, 1, 8},
		{ROWS_PER_STRIP, TYPE_SHORT, 1, y},
		{STRIP_BCOUNTS, TYPE_LONG, 1, total},
		{X_RESOLUTION, TYPE_LONG, 1, 1},
		{Y_RESOLUTION, TYPE_LONG, 1, 1},
		{RESOLUTION_UNIT, TYPE_SHORT, 1, 1} };
	uint16_t num_tags = 11;
	
	int f_size = sizeof(struct tiff_header) + 
			(12 * num_tags) + 	/* tags */
			total + 		/* number of bytes (pixels) */
			sizeof(uint16_t) + 	/* number of tags */
			sizeof(uint32_t);	/* last four bytes are zeroes */
	
	hdr.endianness	= TIFF_ENDIAN_LITTLE;
	hdr.magic	= 0x002a;
	hdr.ifd_offset	= total + 8;
	
	dst_ptr = *out = calloc(f_size, 1);
	
	memcpy(dst_ptr, &hdr, sizeof(struct tiff_header));
	dst_ptr += sizeof(struct tiff_header);
	
	memcpy(dst_ptr, src, total);
	dst_ptr += total;
	
	memcpy(dst_ptr, &num_tags, sizeof(uint16_t));
	dst_ptr += sizeof(uint16_t);
		
	for (i = 0; i < num_tags; i++) {
		memcpy(dst_ptr, &hdrs[i], 12);
		dst_ptr += 12;
	}

	return f_size;
}

/****************************************************************************************/
/*	util functions */
/****************************************************************************************/

/* reads the file at src_path into a buffer allocated in *dst */
int read_file (const char *src_path, unsigned char **dst)
{
	int src_fd;
	int res, pos = 0, rem;
	struct stat st;
	
	if (stat(src_path, &st) < 0 || S_ISDIR(st.st_mode) || (rem = st.st_size) <= 0) {
		fprintf(stderr, "[%s]: stat failed\n", __func__);
		perror("stat");
		return -1;
	}
		
	if ((src_fd = open(src_path, O_RDONLY, 0777)) < 0) {
		fprintf(stderr, "[%s]: could not open socket\n", __func__);
		return -1;
	}
	
	*dst = calloc(1, st.st_size);
	
	while ((res = (int)read(src_fd, (*dst) + pos, rem)) > 0) {
		rem -= res;
		pos += res;
	}
	
	close(src_fd);
	
	fprintf(stderr, "[%s]: read %d bytes\n", __func__, (int)st.st_size);
	
	return (int)st.st_size;
}

int create_file_with_data (const char *dst, unsigned char *src, int bytes)
{
	int src_fd;
	
	if ((src_fd = open(dst, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0)
		return -1;
		
	if (write(src_fd, src, bytes) != bytes)
		return -1;
	
	fprintf(stderr, "[%s]: wrote %d bytes to %s\n", __func__, bytes, dst);

	close(src_fd);
	
	return 0;
}

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

int find_min_index (float *src, char *used, int num)
{
	int min = 0;

	while (num--)
		if (src[num] < src[min] && (!used || (used && !used[num])))
			min = num;
	
	return min; 
}

int find_max_int_index (uint16_t *src, int num)
{
	int max = 0;

	while (--num >= 0)
		if (src[num] > src[max])
			max = num;
	
	return max; 
}

int find_min_int_index (uint16_t *src, char *used, int num)
{
	int min = 0;

	while (num--)
		if (src[num] < src[min] && (!used || (used && !used[num])))
			min = num;
	
	return min; 
}

/****************************************************************************************/
/*	fits header parsing	*/
/****************************************************************************************/
int parse_header_unit (const char in[80], char *title, char *text)
{
	char c;
	int end_pos = 0, i, pos = 0;
	int ret = 0;
	int is_empty = 1;
	
	memset(title, 0, 9);
	memset(text, 0, 80);
	
	for (end_pos = 0; end_pos < 80; end_pos++) {
		if (!in[end_pos] || in[end_pos] == '/' || in[end_pos] == '\n')
			break;
		if (in[end_pos] != ' ')
			is_empty = 0;
	}
	
	if (is_empty)
		return 0;

	for (i = 0; i < 8; i++) {		/* copy the title */
		c = in[i];
		
		if (isupper(c) || isdigit(c) || c == '-' || c == '_') {
			title[i] = c;
		} else if (c == ' ') {
			title[i] = '\0'; 
			break;
		} else {
			fprintf(stderr, "invalid character 0x%.02x\n", c);
			return -1;
		}
	}
	
	if (in[8] != '=' || in[9] != ' ') {
		strncpy(text, in + 8, end_pos - 8);
		return ret;
	}
	
	if (in[10] == '\'' && in[11] == '\'') 		/* data is an empty string */
		return 1;
		
	if (in[10] != '\'') {				/* data is not a string */
		strncpy(text, in + 10, end_pos - 10);
		return ret;
	}
	
	for (i = 0, pos = 11; pos < 80; pos++) {	/* data is a string */
		c = in[pos];
				
		if (c == '\'') {
			if (in[pos + 1] == '\'')
				text[i++] = in[pos++];
			else
				break;
		} else if (c >= 0x20 && c <= 0x7E) {
			text[i++] = c;
		} else {
			fprintf(stderr, "invalid char 0x%.02x\n", c);
			return -1;
		}
	}
	
	text[i] = '\0';

	return 1;
}

int write_json_title (char *file_buf, const char *title, int *buf_pos)
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

int write_num_json (int fd, int nest_level, char *title, double text, int *first)
{
	char file_buf[512];
	int buf_pos = 0;

	if ((nest_level *= 4) > sizeof(file_buf) || !title)
		return -1;
	
	memset(file_buf, 0, sizeof(file_buf));
	if (*first) {
		*first = 0;
	} else {
		file_buf[buf_pos++] = ',';
		file_buf[buf_pos++] = '\n';
	}

	while (nest_level--)
		file_buf[buf_pos++] = ' ';

	write_json_title(file_buf, title, &buf_pos);
	
	buf_pos += snprintf(file_buf + buf_pos, sizeof(file_buf) - buf_pos, "%lf", text);
	
	return -((write(fd, file_buf, buf_pos) != buf_pos));
}	
 
/* only for use with .fits tag reader function */
int write_tag_json (char *file_buf, int is_string, int nest_level, 
			const char *title, const char *text, int *first)
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

int count_fits_tags (const char *src)
{
	int next_line = 0;
	char title[9] = { 0 };
	char text[80] = { 0 };
	int count = 0;
	
	while ((next_line = parse_header_unit(src, title, text)) >= 0) {
		src += 80; 

		if (!strncmp(title, "END", 3))
			break;
		
		++count;
	}

	return count;
}

int read_fits_tags (const char *src, struct fits_convert *fts)
{
	char title[9] = { 0 };
	char text[80] = { 0 };
	int *n_naxis = NULL;
	int cur = 0, tmp = 0, naxis = 0, next_line = 0;
	int retn = 0, out_bytes = 0, first = 1, pos = 0;
	const char *start = src;
	char file_buf[512];
	
	strncpy(fts->metadata + pos, "{\n", 2);
	pos += 2;
	
	while ((next_line = parse_header_unit(src, title, text)) >= 0) {
		src += 80; 
		
		memset(file_buf, 0, sizeof(file_buf));
		out_bytes = write_tag_json(file_buf, next_line, 1, title, text, &first);
		
		if (out_bytes > 0) {
			strncpy(fts->metadata + pos, file_buf, out_bytes);
			pos += out_bytes;
		}
		
		if (!strncmp(title, "SIMPLE", 6)) {
			sscanf(text, "%*[ ]%c", (char *)&tmp);
			
			if (tmp != 'T') {
				printf("unsupported fits type\n");
				return -1;
			}
		} else if (!strncmp(title, "BITPIX", 6)) {
			sscanf(text, "%d", &fts->bitpix);
		} else if (!strncmp(title, "NAXIS", 6)) {
			sscanf(text, "%d", &naxis);
			
			//if (naxis != 2) {
			//	printf("invalid naxis %d\n", naxis);
			//	return -1;
			//}
			
			n_naxis = calloc(sizeof(int), 2);
		} else if (!strncmp(title, "NAXIS", 5) && n_naxis) {
			sscanf(title + 5, "%d", &cur);
			sscanf(text, "%d", &tmp);
			n_naxis[cur - 1] = tmp;
		} else if (!strncmp(title, "END", 3)) {
			retn = src - start;
			break;
		}
		
		memset(title, 0, sizeof(title));
		memset(text, 0, sizeof(text));
	}
	
	strncpy(fts->metadata + pos, "\n}", 2);
	pos += 2;
	
	fts->x = n_naxis[0];	/* x */	
	fts->y = n_naxis[1];	/* y */
	
	free(n_naxis);
	
	fts->metadata_size = pos;
	
	fprintf(stderr, "[%s]: read fits tags, (%d x %d) at %d bits/pix\n", __func__,
		fts->x, fts->y, fts->bitpix);
	
	return retn;
}

/* allocates *dst and populates it with the smallest k values from num values in src */
int find_lowest_x_floats (float *src, int num, float **dst, int k)
{
	int i, o_pos = 0, mi;
	char *used; 
	
	if (k > num || !src || !dst)
		return -1;
		
	used = calloc(1, num);
	*dst = calloc(sizeof(float), k);
	
	for (i = 0; i < k; i++) {
		mi = find_min_index(src, used, num);
			
		(*dst)[o_pos++] = src[mi];
		used[mi] = 1;
	}
	
	free(used);

	return 0;
}

/* writes a string in .json format to fd: "title": "text" */
int write_json_str_tof (const char *title, const char *text, int *first, int fd)
{
	int out_len;
	char tmp_buf[512] = { 0 };
	
	if ((out_len = write_tag_json(tmp_buf, 1, 1, title, text, first)) <= 0)
		return -1;
		
	return (write(fd, tmp_buf, out_len) == out_len) ? 0 : -1;
}

/* writes "k" lowest values from time_array, returns average of those values */
double write_k_lowest_values (float *time_array, int runs, int k, int *first, int fd)
{
	float *low;
	int i;
	double avg_lowest = 0;
	char title[32];
	
	find_lowest_x_floats(time_array, runs, &low, k);
	
	/* add number of run */
	for (i = 0; i < k; i++) {
		snprintf(title, sizeof(title), "run #%d (usec)", i);
		if (write_num_json(fd, 1, title, (double)low[i], first) < 0)
			return 0;
			
		avg_lowest += (low[i] / k);
	}

	free(low);

	return avg_lowest;
}

/* writes timing data as .json, "k" is the number of quickest runs to save */
int write_time_file (const char *path, float *time_array, int runs, int k)//, struct fits_convert fts)
{ 
	int first = 1, time_fd;
	double avg_lowest = 0;
	struct utsname un;
	time_t ts_val;
	char time_buf[80];
	
	if ((time_fd = open(path, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0) {
		fprintf(stderr, "[%s]: error opening socket\n", __func__);
		return -1;
	}
		
	if (write(time_fd, "{\n", 2) != 2 ||
		write_num_json(time_fd, 1, "total # runs", (double)runs, &first) < 0 ||
		write_num_json(time_fd, 1, "# runs saved", (double)k, &first) < 0)
		fprintf(stderr, "[%s]: error writing run info\n", __func__);
		
	if (!(avg_lowest = write_k_lowest_values(time_array, runs, k, &first, time_fd)))
		fprintf(stderr, "[%s]: error writing run times\n", __func__);	
	
	if (uname(&un) ||
		write_json_str_tof("sysname", un.sysname, &first, time_fd) ||
		write_json_str_tof("release", un.release, &first, time_fd) ||
		write_json_str_tof("machine", un.machine, &first, time_fd))
		fprintf(stderr, "[%s]: error writing system info\n", __func__);

	if (time(&ts_val) < 0 ||
		!strftime(time_buf, 80, "%x - %I:%M%p", localtime(&ts_val))  ||
		write_json_str_tof("time", time_buf, &first, time_fd))
		fprintf(stderr, "[%s]: error writing stop time\n", __func__);
 
	if (write_num_json(time_fd, 1, "avg. microseconds", avg_lowest, &first) < 0 ||
	        write(time_fd, "\n}", 2) != 2)
		/*write_num_json(time_fd, 1, "input bytes", (double)fts.input_size, &first) < 0 ||
		write_num_json(time_fd, 1, "width (pixels)", (double)fts.x, &first) < 0 ||
		write_num_json(time_fd, 1, "height (pixels)", (double)fts.y, &first) < 0 ||
		write_num_json(time_fd, 1, "bits/pixel", (double)fts.bitpix, &first) < 0 ||
		write(time_fd, "\n}", 2) != 2)*/
		fprintf(stderr, "[%s]: error writing image data\n", __func__);

	close(time_fd);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
} 

/* research compression methods and find ease of decoding,  */
/* find larger test images */

void u16_to_u8_scaled (uint16_t *src, unsigned char *dst, int num)
{
	int i, x;
	double sum2 = 0, sum3 = 0, mean = 0;
	double variance2, ratio, tmp;
	uint16_t min, max;

	min = src[find_min_int_index(src, NULL, num)];
	max = src[find_max_int_index(src, num)];

	for (x = 0; x < num; x++)
		mean += (double)((double)src[x] / (double)num);	
	
	for (x = 0; x < num; x++) {
		tmp = ((double)src[x] - mean);
		sum2 += tmp * tmp;
		sum3 += tmp;
	}
	 
	variance2 = (sum2 - ((sum3 * sum3) / (double)num)) / ((double)num);
	ratio = pow(max, 3) / 255.0f; 
	
	fprintf(stderr, "[%s]: range = (%d -> %d), mean = %lf, variance = %lf ratio = %lf\n",
		__func__, min, max, mean, variance2, ratio);

	for (i = 0; i < num; i++) {
		dst[i] = (unsigned char)((double)(pow(src[i], 3) / ratio));
		//printf("src = %d dst = %d  log src = %lf / ratio = (%lf)\n", src[i], dst[i], pow(src[i], 2), pow(src[i], 2) / ratio);
	}
}

void f32_to_u8_scaled (float *big_endian, unsigned char *dst, int num)
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

	min = src[find_min_index(src, NULL, num)];
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

	for (i = 0; i < num; i++)
		dst[i] = (log(src[i]) / ratio);

	free(src);
}

#define NUM 256
#define KERNEL_SOURCE \
"float switch_endian (float inp)					\n" \
"{									\n" \
"	char c, *ptr = (char *)&inp;					\n" \
"	c = ptr[0], ptr[0] = ptr[3], ptr[3] = c;			\n" \
"	c = ptr[1], ptr[1] = ptr[2], ptr[2] = c;			\n" \
"	return inp;							\n" \
"}									\n" \
"__kernel void vadd(__global float *big_endian,				\n" \
"		__global int *sizes,					\n" \
"		__global unsigned char * dst, 				\n" \
"		__global unsigned char * hist0,				\n" \
"		__global unsigned char * hist1,				\n" \
"		__local int *counts,					\n" \
"		__local float *imax,					\n" \
"		__global int *test)					\n" \
"{                                                                      \n" \
"	int glid = get_group_id(0); 					\n" \
"	int lsize = sizes[glid] / get_local_size(0);			\n" \
"	int start = get_local_id(0) * lsize;				\n" \
"	int total_size = 0;						\n" \
"	float tmp, mt;							\n" \
"	int hstart = glid * 256 * 256, x, maxp = 1, ind;		\n" \
"	int i, max = 0, tmpl, off = ((256 - 1) * 256), pos;		\n" \
"	for (x = 0; x < get_num_groups(0); x++)				\n" \
"		total_size += sizes[x];					\n" \
"	int avg_chunk = (total_size / get_global_size(0));		\n" \
"	int avg_start = avg_chunk * get_global_id(0);			\n" \
"	int avg_end = avg_start + avg_chunk;				\n" \
"	if (get_global_id(0) + 1 == get_global_size(0))			\n" \
"		avg_end = total_size;					\n" \
"	for (x = avg_start; x < avg_end; x++)				\n" \
"		big_endian[x] = switch_endian(big_endian[x]);		\n" \
"	barrier(CLK_GLOBAL_MEM_FENCE);					\n" \
"	for (x = 0; x < glid; x++)					\n" \
"		start += sizes[x];					\n" \
"	barrier(CLK_LOCAL_MEM_FENCE);					\n" \
"	if (get_local_id(0) == 0) {					\n" \
"		*imax = (big_endian[start]);				\n" \
"		for (i = 0; i < 256; i++)				\n" \
"			counts[i] = 0;					\n" \
"	}								\n" \
"	barrier(CLK_LOCAL_MEM_FENCE);					\n" \
"	for (x = 0; x < lsize; x++) {					\n" \
"		if (big_endian[x + start] > *imax)			\n" \
"			*imax = big_endian[x + start];			\n" \
"	}	 							\n" \
"	barrier(CLK_LOCAL_MEM_FENCE);					\n" \
"	tmp = log(*imax) / 255.0f;					\n" \
"	for (x = 0; x < lsize; x++) {					\n" \
"		mt = (float)(log(big_endian[start + x]) / tmp);		\n" \
"		dst[start + x] = (unsigned char)mt;			\n" \
"	}								\n" \
"	for (i = 0; i < lsize; i++)					\n" \
"		counts[dst[start + i]]++;				\n" \
"	if (get_local_id(0) != 0)					\n" \
"		return;							\n" \
"	for (i = 0; i < 256; i++)					\n" \
"		if (counts[max] < counts[i])				\n" \
"			max = i;					\n" \
"	for (mt = (float)counts[max], i = 0; i < 256; i++) {		\n" \
"		tmpl = (int)((float)(256 * ((float)counts[i] / mt))); 	\n" \
"		for (pos = off + i; tmpl > 0; tmpl--, pos -= 256)	\n" \
"			hist0[pos + hstart] = 128;			\n" \
"	}								\n" \
"	for (i = 0; i < 256; i++)					\n" \
"		if (i != max && (counts[maxp] < counts[i]))		\n" \
"			maxp = i;					\n" \
"	for (mt = (float)counts[maxp], i = 0; i < 256; i++) {		\n" \
"		ind = ((i == max) ? maxp : i);				\n" \
"		tmpl = (int)((float)(256 * ((float)counts[ind] / mt)));	\n" \
"		for (pos = off + i; tmpl > 0; tmpl--, pos -= 256)	\n" \
"			hist1[pos + hstart] = 128;			\n" \
"	}								\n" \
"}									\n\n"

int make_histogram (unsigned char *buf, unsigned char *data, int size, 
		int height, int omit_largest)
{
	unsigned char *ptr;
	int i, max = 0, maxp = 1, ind, tmp, off = ((height - 1) * NUM);
	int counts[NUM] = { 0 };
	
	for (i = 0; i < size; i++)
		counts[data[i]]++;
	for (i = 0; i < NUM; i++)
		if (counts[max] < counts[i])
			max = i;
	   
	if (!omit_largest)
		maxp = max;
	else
		for (i = 0; i < NUM; i++)
			if (i != max && (counts[maxp] < counts[i]))
				maxp = i;

	for (i = 0; i < NUM; i++) {
		ind = (!omit_largest) ? i : ((i == max) ? maxp : i);
		tmp = (int)((float)(height * ((float)counts[ind] / (float)counts[maxp])));
		for (ptr = buf + off + i; tmp > 0; tmp--, ptr -= NUM)
			*ptr = 128;
	}
	
	return tmp;
} 

//gcc -framework OpenCL -DAPPLE fits_to_tiff.c; ./a.out fits\ examples

int read_cl_buf (cl_command_queue commands, cl_mem mem, int memsz, void *dst)
{
	int err;
	
	if ((err = clEnqueueReadBuffer(commands, mem, CL_TRUE, 0, memsz, 
    		dst, 0, NULL, NULL)) == CL_SUCCESS)
    		return 0;

        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        return -1;
}
    

int do_f32convert_cl_bulk (struct fits_convert *array, int *sizes, int num)
{
	int gsize = 16;
	int i, j, err, pos = 0, hpos = 0;
	size_t global = num * gsize, local = gsize, memsz = 0;
	unsigned char *out;
	cl_device_id     device_id;
	cl_context       context;
	cl_command_queue commands;
	cl_program       program;
	cl_kernel        ko_vadd;
	cl_mem d_a, d_d, d_b, h0, h1, test;
	cl_uint numPlatforms;
	int hist_buf_sz = NUM * NUM * num;
	unsigned char *h0_tmp = calloc(hist_buf_sz, 1);
	unsigned char *h1_tmp = calloc(hist_buf_sz, 1);
	float *data_big;
	const char *KernelSource = KERNEL_SOURCE;
	
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif
	for (j = 0; j < num; j++) {
		if (array[j].bitpix != -32 || !array[j].img_out) 
			sizes[j] = 0;
		memsz += sizes[j];
		printf("num %d j %d sz %lu\n", num, j, memsz);
	}

	if (!(data_big = calloc(memsz, sizeof(float))) || !(out = calloc(memsz, 1))) {
		perror("alloc");
		return -1;
	}
	
	for (j = 0; j < num; j++) {
		if (array[j].bitpix != -32 || !array[j].img_out) 
			continue;
		printf("pos = %d / %lu + %d %d\n", pos, memsz * sizeof(float), 
			sizes[j], array[j].img_size);
		memcpy(data_big + pos, (array[j].img_out + array[j].img_size), 
			sizes[j] * sizeof(float));
		pos += sizes[j];
	}

	// Find number of platforms
	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	checkError(err, "Finding platforms");
	if (numPlatforms == 0) {
		printf("Found 0 platforms!\n");
		return EXIT_FAILURE;
	}
	// Get all platforms
	cl_platform_id pt[numPlatforms];
	err = clGetPlatformIDs(numPlatforms, pt, NULL);
	checkError(err, "Getting platforms");

	// Secure a GPU
	for (i = 0; i < numPlatforms; i++)
		if ((err = clGetDeviceIDs(pt[i], DEVICE, 1, &device_id, NULL)) == CL_SUCCESS)
			break;

	if (device_id == NULL)
	checkError(err, "Finding a device");

	char name[512];
	clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(name), name, NULL);
	printf("\nUsing OpenCL device: %s\n", name);

	cl_ulong sz;
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(sz), &sz, NULL);
	printf("\nmax size %llu, requested %lu\n", sz, memsz);

	// Create a compute context
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	checkError(err, "Creating context");

	// Create a command queue
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	checkError(err, "Creating command queue");

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1, 
		(const char **)&KernelSource, NULL, &err);
	checkError(err, "Creating program");
 
    // Build the program
    if ((err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL)) != CL_SUCCESS) {
        size_t len;
        char buffer[2048];
        printf("Error: Failed to build program executable!\n%s\n", err_code(err));
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 
        			sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    ko_vadd = clCreateKernel(program, "vadd", &err);
    checkError(err, "Creating kernel");

    d_a  = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR, 
    		memsz * sizeof(float), data_big, &err);
    checkError(err, "Creating buffer d_a");
    d_b  = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 
    		sizeof(int) * num, sizes, &err);
    checkError(err, "Creating buffer d_a");

    d_d  = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, memsz, NULL, &err);
    checkError(err, "Creating buffer d_d");

    h0  = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, hist_buf_sz, NULL, &err);
    checkError(err, "Creating buffer h0");
    
    h1  = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, hist_buf_sz, NULL, &err);
    checkError(err, "Creating buffer h1");

    test = clCreateBuffer(context,  CL_MEM_WRITE_ONLY, sizeof(int) * 10, NULL, &err);
    checkError(err, "Creating buffer h1");


    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_d);
    err |= clSetKernelArg(ko_vadd, 3, sizeof(cl_mem), &h0);
    err |= clSetKernelArg(ko_vadd, 4, sizeof(cl_mem), &h1);
    err |= clSetKernelArg(ko_vadd, 5, 256 * sizeof(int), NULL);
    err |= clSetKernelArg(ko_vadd, 6, sizeof(float), NULL);
    err |= clSetKernelArg(ko_vadd, 7, sizeof(cl_mem), &test);
    checkError(err, "Setting kernel arguments");

    err = clEnqueueNDRangeKernel(commands, ko_vadd, 1, NULL, 
    		&global, &local, 0, NULL, NULL);
    checkError(err, "Enqueueing kernel");

    // Wait for the commands to complete before stopping the timer
    err = clFinish(commands);
    checkError(err, "Waiting for kernel to finish");

    // Read back the results from the compute device
    if ((err = clEnqueueReadBuffer(commands, d_d, CL_TRUE, 0, memsz, 
    		out, 0, NULL, NULL)) != CL_SUCCESS) {
        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        return -1;
    }
    
    if ((err = clEnqueueReadBuffer(commands, h0, CL_TRUE, 0, hist_buf_sz, 
    		h0_tmp, 0, NULL, NULL)) != CL_SUCCESS) {
        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        return -1;
    }
    
    if ((err = clEnqueueReadBuffer(commands, h1, CL_TRUE, 0, hist_buf_sz, 
    		h1_tmp, 0, NULL, NULL)) != CL_SUCCESS) {
        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        return -1;
    }
    
    
    int test_out[10] = { 0 };
    if ((err = clEnqueueReadBuffer(commands, test, CL_TRUE, 0, sizeof(test_out), 
    		test_out, 0, NULL, NULL)) != CL_SUCCESS) {
        printf("Error: Failed to read output array!\n%s\n", err_code(err));
        return -1;
    }
    
    printf("%d groups chunk %d start %d end %d total %d group %d\n", test_out[0], 
    	test_out[1], test_out[2], test_out[3], test_out[4], test_out[5]);
    
	pos = 0;
	for (i = 0; i < num; i++) {
		if (array[i].bitpix != -32 || !array[i].img_out || !sizes[i]) {
			printf("skipp %d\n", i); 
			continue;
		}
		array[i].hist0 = calloc(NUM, NUM);
		array[i].hist1 = calloc(NUM, NUM);
		memcpy(array[i].hist0, h0_tmp + hpos, NUM * NUM);
		memcpy(array[i].hist1, h1_tmp + hpos, NUM * NUM);
		memcpy(array[i].img_out, out + pos, sizes[i]); 
		hpos += NUM * NUM;
		pos += sizes[i];
	}
	clReleaseMemObject(test);
	clReleaseMemObject(d_a);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_d);
	clReleaseMemObject(h0);
	clReleaseMemObject(h1);
	clReleaseProgram(program);
	clReleaseKernel(ko_vadd);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);
	free(out);
	free(data_big);
	free(h0_tmp);
	free(h1_tmp);

	return 0;
}

void save_all (struct fits_convert *array, int num)
{
	int j, len;
	char buf[512];
	
	for (j = 0; j < num; j++) {
		if (!array[j].path_input || !array[j].img_out || array[j].bitpix != -32)
			continue;
		memset(buf, 0, sizeof(buf));
		len = (int)strlen(array[j].path_input);
		strncpy(buf, array[j].path_input, sizeof(buf));

		strcpy(buf + len, "_histo_full.tiff");
		create_file_with_data(buf, array[j].hist0, array[j].hs0);

		strcpy(buf + len, "_histo_cutoff.tiff");
		create_file_with_data(buf, array[j].hist1, array[j].hs1);

		strcpy(buf + len, ".tiff");
		create_file_with_data(buf, array[j].img_out, array[j].img_size);
		
		strcpy(buf + len, "_metadata.json");
		create_file_with_data(buf, (unsigned char *)array[j].metadata, 
			array[j].metadata_size);

		free(array[j].metadata);
		free(array[j].img_out);
		free(array[j].hist0);
		free(array[j].hist1);
		free((void *)array[j].path_input);
	}
}

int fill_fits_array (struct fits_convert **dst, int **sizes, const char *path)
{
	struct fits_convert *ptr;
	int count = 0, ind = 0;
	struct dirent *ds;
	DIR *dp;
	char path_buf[256] = { 0 };
	
	if (!(dp = opendir(path)))
		return -1;

	while ((ds = readdir(dp)))
		if (!strcmp(".fits", ds->d_name + strlen(ds->d_name) - 5))
			if (strlen(ds->d_name) > 5)
				++count;
	closedir(dp);
	
	*dst = calloc(sizeof(struct fits_convert), count);
	*sizes = calloc(sizeof(int), count);
	
	if (!(dp = opendir(path)))
		return -1;

	while ((ds = readdir(dp))) {
		if (strcmp(".fits", ds->d_name + strlen(ds->d_name) - 5))
			continue;
			
		if (strlen(ds->d_name) <= 5)
			continue;

		memset(path_buf, 0, sizeof(path_buf));
		snprintf(path_buf, sizeof(path_buf), "%s/%s", path, ds->d_name);
		
		ptr = &((*dst)[ind]);
		ptr->path_input = strdup(path_buf);
		
		if ((ptr->input_size = (int)read_file(path_buf, &ptr->img_out)) < 0) {
			fprintf(stderr, "[%s]: invalid file %s\n", __func__, path_buf);
			continue;
		}

		if ((ptr->metadata_size = count_fits_tags((const char *)ptr->img_out)) < 0) {
			fprintf(stderr, "[%s]: could not count fits tags\n", __func__);
			continue;
		}

		ptr->metadata = calloc(ptr->metadata_size, 81);

		if ((ptr->img_size = read_fits_tags((const char *)ptr->img_out, ptr)) < 0) {
			fprintf(stderr, "[%s]: could not read fits tags\n", __func__);		
			continue;
		}
		
		if (ptr->bitpix != -32) {
			fprintf(stderr, "[%s]: invalid pixel size\n", __func__);
			free(ptr->metadata);
			free(ptr->img_out);
			free((void *)ptr->path_input);
			memset(ptr, 0, sizeof(struct fits_convert));
			continue;
		}

		(*sizes)[ind] = ptr->x * ptr->y;
		fprintf(stderr, "[%s]: read file %s size %d\n", __func__, 
			path_buf, (*sizes)[ind]);
		ind++;
	}
	closedir(dp);

	return ind;

}

int do_run_cl (struct fits_convert *array, int *sizes, int num)
{
	unsigned char *buf = NULL; 
	unsigned char *hbuf = calloc(NUM, NUM + 1);
	unsigned char *hbuf1 = calloc(NUM, NUM + 1);
	int i, j;

	do_f32convert_cl_bulk(array, sizes, num);

	for (j = 0; j < num; j++) {
		printf("%d: %s (pix %d %d x %d) %d\n", j, array[j].path_input, 
			array[j].bitpix, array[j].x, array[j].y, sizes[j]);
		memset(hbuf, 0, NUM * NUM);
		memset(hbuf1, 0, NUM * NUM);

		if (!array[j].img_out || !array[j].hist0)
			continue;

		memcpy(hbuf, array[j].hist0, NUM * NUM);
		memcpy(hbuf1, array[j].hist1, NUM * NUM);
		free(array[j].hist0);
		free(array[j].hist1);

		array[j].img_size = write_tiff_file(&buf, array[j].img_out, 
				array[j].x, array[j].y);
		free(array[j].img_out);
		array[j].img_out = buf;		

		array[j].hs0 = write_tiff_file(&array[j].hist0, hbuf, NUM, NUM);
		array[j].hs1 = write_tiff_file(&array[j].hist1, hbuf1, NUM, NUM);
	}
	
	free(hbuf);
	free(hbuf1);
	
	return 0;
}

int do_run (struct fits_convert *array, int *sizes, int num)
{
	unsigned char *buf = NULL; 
	unsigned char *hbuf = calloc(NUM, NUM + 1);
	unsigned char *hbuf1 = calloc(NUM, NUM + 1);
	int i, j;

	for (j = 0; j < num; j++) {
		printf("%d: %s (pix %d %d x %d) %d\n", j, array[j].path_input, 
			array[j].bitpix, array[j].x, array[j].y, sizes[j]);
		memset(hbuf, 0, NUM * NUM);
		memset(hbuf1, 0, NUM * NUM);

		if (array[j].bitpix != -32 || !sizes[j])
			continue;
			
		buf = calloc(array[j].x, array[j].y);

		f32_to_u8_scaled((float *)(array[j].img_out + array[j].img_size), 
				buf, sizes[j]);

		free(array[j].img_out);

		make_histogram(hbuf, buf, sizes[j], NUM, 0);
		make_histogram(hbuf1, buf, sizes[j], NUM, 1);
		
		array[j].img_size = write_tiff_file(&array[j].img_out, buf, 
				array[j].x, array[j].y);
	
		free(buf);

		array[j].hs0 = write_tiff_file(&array[j].hist0, hbuf, NUM, NUM);
		array[j].hs1 = write_tiff_file(&array[j].hist1, hbuf1, NUM, NUM);
	}
	
	free(hbuf);
	free(hbuf1);
	
	return 0;
}

#define USE_CL

int main (int argc, const char **argv)
{
	struct fits_convert *array;//fts;
	unsigned char *buf = NULL; 
	int i, k = 1, runs = 1, *sizes, j, num, len = (int)strlen(argv[1]); 
	float *time_array = calloc(runs, sizeof(float));
	struct timeval t0, t1;
	char path[512];

	for (i = 0; i < runs; i++) {
		num = fill_fits_array(&array, &sizes, argv[1]);
		gettimeofday(&t0, NULL);
#ifdef USE_CL
		do_run_cl(array, sizes, num);
#else
		do_run(array, sizes, num);
#endif
		gettimeofday(&t1, NULL);
		time_array[i] = (t1.tv_sec - t0.tv_sec) * 1000000LL + 
				t1.tv_usec - t0.tv_usec;
		save_all(array, num);
		free(array);
		free(sizes);
	}

	strncpy(path, argv[1], sizeof(path));
	strcpy(path + len, "_time.json"); 
	printf("writing %s\n", path);
	write_time_file(path, time_array, runs, k);
	
	free(time_array);
	
	return 0;
}

/* 
32 bit floats				
Input bytes	Avg microseconds	x	y	pixels
63360	681.333334	100	100	10000
161280	1202.699978	192	192	36864
10301760	20460.33337	800	800	640000
4219200	34946.60034	1024	1024	1048576
4521600	38399.90027	1055	1061	1119355
7951680	69234.73364	1411	1406	1983866
7957440	69188.80029	1409	1409	1985281
10296000	82810.96704	1600	1600	2560000
10293120	82947.20044	1600	1600	2560000
10298880	81920.69971	1600	1600	2560000
16804800	143117.4009	2048	2048	4194304
				
16 bit ints				
Input bytes	Avg microseconds	x	y	pixels
538560	7006.233337	512	512	262144
5158080	19408.06708	800	800	640000
2125440	30701.33325	1024	1024	1048576
11327040	176269.4336	2376	2381	5657256
*/
