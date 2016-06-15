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

struct fits_convert {
	const char *path_input;
	const char *path_tiff;
	const char *path_metadata;
	const char *path_timedata;
	
	int x, y;
	int bitpix;
	
	char *metadata;
	int metadata_size;
	
	unsigned char *img_out;
	int img_size;
};

/****************************************************************************************/
/*	tiff writer	*/
/****************************************************************************************/
struct tiff_header {
	uint16_t endianness;
	uint16_t magic;
	uint32_t ifd_offset;
};

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

struct ifd_header {
	uint16_t tag_type;
	uint16_t field_type;
	uint32_t num_values;
	uint32_t value;	
};

int write_tiff_file (unsigned char **out, unsigned char *src, int x, int y)
{
	int total = (x * y), i;
	struct tiff_header hdr;
	unsigned char *dst_ptr; 
	struct ifd_header hdrs[11] = {
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

int write_num_json (int fd, int is_string, int nest_level, char *title, 
			double text, int *first)
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
	 
	len = snprintf(file_buf + buf_pos, sizeof(file_buf) - buf_pos, "%lf", text);
		
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
	int retn = 0, out_bytes, first = 1, pos = 0;
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
			
			if (naxis != 2) {
				printf("invalid naxis %d\n", naxis);
				return -1;
			}
			
			n_naxis = calloc(sizeof(int), 2);
		} else if (!strncmp(title, "NAXIS", 5) && n_naxis) {
			sscanf(title + 5, "%d", &cur);
			sscanf(text, "%d", &tmp);
			n_naxis[cur - 1] = tmp;
		} else if (!strncmp(title, "END", 3)) {
			retn = src - start;
			break;
		}
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
	
	if (k >= num || !src || !dst)
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
	char tmp_buf[512];
	
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
		if (write_num_json(fd, 0, 1, title, (double)low[i], first) < 0)
			return 0;
			
		avg_lowest += (low[i] / k);
	}

	free(low);

	return avg_lowest;
}

/* writes timing data as .json, "k" is the number of quickest runs to save */
int write_time_file (const char *path, float *time_array, int runs, 
			int k, struct fits_convert fts)
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
		write_num_json(time_fd, 0, 1, "total # runs", (double)runs, &first) < 0 ||
		write_num_json(time_fd, 0, 1, "# runs saved", (double)k, &first) < 0)
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
 
	if (write_num_json(time_fd, 0, 1, "avg. microseconds", avg_lowest, &first) < 0 ||
		write_num_json(time_fd, 0, 1, "width (pixels)", (double)fts.x, &first) < 0 ||
		write_num_json(time_fd, 0, 1, "height (pixels)", (double)fts.y, &first) < 0 ||
		write_num_json(time_fd, 0, 1, "bits/pixel", (double)fts.bitpix, &first) < 0 ||
		write(time_fd, "\n}", 2) != 2)
		fprintf(stderr, "[%s]: error writing image data\n", __func__);

	close(time_fd);
	
	fprintf(stderr, "[%s]: wrote timing data\n", __func__);
	
	return 0;
} 

/* research compression methods and find ease of decoding,  */
/* find larger test images */

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

	for (i = 0; i < num; i++) {
		dst[i] = (log(src[i]) / ratio);
		
		//printf("src = %lf, ratio=%lf min = %f, max = %f, dst = %d log=%lf\n", 
			//src[i], ratio, min, max, dst[i], log(src[i]) / ratio);
	}
	
	free(src);
}

/* returns execution time in microseconds */
uint64_t run_conversion (unsigned char *buf, int file_size, struct fits_convert *fts)
{
	int datapos = 0;
	unsigned char *data;
	struct timeval t0, t1;
	
	gettimeofday(&t0, NULL);
	
	if ((fts->metadata_size = count_fits_tags((const char *)buf)) < 0) {
		fprintf(stderr, "[%s]: could not count fits tags\n", __func__);
		return 0;
	}
	
	fts->metadata = calloc(fts->metadata_size, 81);
	
	if ((datapos = read_fits_tags((const char *)buf, fts)) < 0) {
		fprintf(stderr, "[%s]: could not read fits tags\n", __func__);		
		return 0;
	}
	
	data = calloc(fts->x, fts->y);

	f32_to_u8_scaled((float *)(buf + datapos), data, fts->x * fts->y);

	fts->img_size = write_tiff_file(&fts->img_out, data, fts->x, fts->y);
		
	free(data);
		
	gettimeofday(&t1, NULL);
	
	return (t1.tv_sec - t0.tv_sec) * 1000000LL + t1.tv_usec - t0.tv_usec;
}

int main (void)
{
	struct fits_convert fts;
	unsigned char *buf; 
	int file_size, i;
	uint64_t usec;
	int k = 30, runs = 50;
	float *time_array = calloc(runs, sizeof(float));
	
	memset(&fts, 0, sizeof(struct fits_convert));
	
	fts.path_input = "/Users/nobody1/Desktop/green.fits";
	fts.path_tiff = "/Users/nobody1/Desktop/test.tiff";
	fts.path_metadata = "/Users/nobody1/Desktop/test.json";
	fts.path_timedata = "/Users/nobody1/Desktop/time.json";

	if ((file_size = (int)read_file(fts.path_input, &buf)) < 0)
		return -1;

	for (i = 0; i < runs; i++) {
		free(fts.img_out);
		free(fts.metadata);
		usec = run_conversion(buf, file_size, &fts);
		time_array[i] = (float)usec;
	}
	
	if (write_time_file(fts.path_timedata, time_array, runs, k, fts) < 0)
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
