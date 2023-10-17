#ifndef FILEACT_H
#define FILEACT_H

#include <stdio.h>

int does_file_exist(FILE *file_ptr, const char *file_name);
int create_file(FILE *file_ptr, const char *file_name, const char *fmt, const char *header);
int write_to_file(FILE *file_ptr, const char *fmt, ...);
int verify_line_from_file(FILE *file_ptr, const char *correct_line, const int line_max);

#endif