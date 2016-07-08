#ifndef JSON_DATA_H
#define JSON_DATA_H
#include <stdio.h>
#include <stdlib.h>

int find_min_index(float * src, int num);

int write_json_label (char *file_buf, char *title, int *buf_pos, int nest_level);

int write_json_title(char * file_buf, char * title, int *buf_pos, int nest_level);

int write_num_json(FILE* dfp,int nest_level, char *title, double value, int *first);

int write_tag_json(char *file_buf,int nest_level, char *title, char *text, int *first);

int write_super_file (double **data_arr, char **label_arr, long num_labels, long num_runs, char * opsSeq, int * numSeq, int maxNumOps, char* input, char* output, int inputSize);

#endif
