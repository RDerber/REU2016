#ifndef JSON_DATA_H
#define JSON_DATA_H
#include <stdio.h>
#include <stdlib.h>

int find_min_index(float * src, int num);

int write_json_label (char *file_buf, char *title, int *buf_pos, int nest_level);

int write_json_title (FILE * dfp, int nest_level, char *title, int *first);

int write_num_json(FILE* dfp,int nest_level, char *title, double value, int *first);

int write_tag_json(FILE* dfp,int nest_level, char *title, char *text, int *first);

int write_super_file (double **data_arr, char **label_arr, int num_labels, int *runs_arr, char * ops_seq, int * num_seq, int max_num_ops, char* input, char* output, int input_size);

int write_time_file (double **data_arr, char **label_arr, int num_labels, int num_runs);

int write_DAO_file (double **data_arr, char **label_arr, int num_labels, int *runs_arr, char * input, char * output, int keySize);

int write_laGrange_file (double **data_arr, char **label_arr, int num_labels, int num_runs, int* input, int* output, int inputSize);

#endif
