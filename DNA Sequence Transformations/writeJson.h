#ifndef WRITE_JSON_H
#define WRITE_JSON_H

int find_min_index(float * src, int num);

int write_json_title(char * file_buf, char * title, int *buf_pos);

int write_num_json(int fd, int is_string, int nest_level, char *title, double value, int *first);

int write_tag_json(char *file_buf, int is_string, int nest_level, char *title, char *text
, int *first);

int find_lowest_x_floats(float *src, int num, float **dst, int k);

/* k is the number of minimum values to be compared in the time report*/
int write_time_file (float *time_array, int runs, int k, long inputsize);

#endif
