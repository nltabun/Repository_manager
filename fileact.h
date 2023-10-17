#ifndef FILEACT_H
#define FILEACT_H

int does_file_exist(const char *file_name);
int create_file(const char *file_name);
int write_to_file(const char *file_name, const char *fmt, ...);

#endif