#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "fileact.h"

int does_file_exist(FILE *file_ptr, const char *file_name);
int create_file(FILE *file_ptr, const char *file_name, const char *fmt);
int write_to_file(FILE *file_ptr, const char *fmt, ...);
int verify_line_from_file(FILE *file_ptr, const char *correct_line, const int line_max);

int does_file_exist(FILE *file_ptr, const char *file_name)
{
    file_ptr = fopen(file_name, "r");

    if (file_ptr == NULL)
        return 0;

    fclose(file_ptr);
    
    return 1;
}

int create_file(FILE *file_ptr, const char *file_name, const char *fmt)
{
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL)
        return 0;

    fclose(file_ptr);

    return 1;
}

int write_to_file(FILE *file_ptr, const char *fmt, ...)
{
    int result = 0;   
    va_list args;
    va_start(args, fmt);
    if (vfprintf(file_ptr, fmt, args) >= 0)
    {
        result = 1;
        fprintf(file_ptr, "\n");
    }
    va_end(args);
    
    return result;
}

// Read a line from file and verify whether it matches the line given as a parameter.
int verify_line_from_file(FILE *file_ptr, const char *correct_line, const int line_max)
{
    char line[line_max];
    if (fgets(line, sizeof(line), file_ptr) == NULL)
        return 0;

    line[strcspn(line, "\n")] = 0;
    if (strcmp(line, correct_line) != 0)
        return 0;
    
    return 1;
}