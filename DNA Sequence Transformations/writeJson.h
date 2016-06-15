#ifndef WRITE_JSON_H
#define WRITE_JSON_H

int find_min_index(float *, int);

int write_json_title(char *, char *, int *);

int write_num_json(int, int, int, char *, double, int *);

int write_tag_json(char *, int, int, char *, char *, int *);

int find_lowest_x_floats(float *, int, float **, int);

int write_time_file (float *, int, int, long);

#endif
