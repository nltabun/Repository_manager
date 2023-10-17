#include <stdio.h>
#include <stdarg.h>
#include "fileact.h"

int does_file_exist(const char *file_name);
int create_file(const char *file_name);
int write_to_file(const char *file_name, const char *fmt, ...);

int does_file_exist(const char *file_name)
{
    FILE *file_ptr;
    file_ptr = fopen(file_name, "r");

    if (file_ptr == NULL)
        return 0;

    fclose(file_ptr);
    
    return 1;
}

int create_file(const char *file_name)
{
    FILE *file_ptr;
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL)
        return 0;    

    fclose(file_ptr);

    return 1;
}

int write_to_file(const char *file_name, const char *fmt, ...)
{
    FILE *file_ptr;
    file_ptr = fopen(file_name, "w");
    
    if (file_ptr == NULL)
        return -1;

    int result = 0;   
    va_list args;
    va_start(args, fmt);
    result = vfprintf(file_ptr, fmt, args);

    va_end(args);
    
    return result;
}