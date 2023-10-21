#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "fileact.h"

int does_file_exist(FILE *file_ptr, const char *file_name);
int create_file(FILE *file_ptr, const char *file_name);
int write_to_file(FILE *file_ptr, const char *fmt, ...);
int verify_line_from_file(FILE *file_ptr, const char *correct_line, const int line_max);

// Function to check if a file exists by trying to open it
// Return 1 on success, 0 on failure
int does_file_exist(FILE *file_ptr, const char *file_name)
{
    file_ptr = fopen(file_name, "r");

    if (file_ptr == NULL)
        return 0;

    fclose(file_ptr);
    
    return 1;
}

// Function to create a file
// Return 1 on success, 0 on failure
int create_file(FILE *file_ptr, const char *file_name)
{
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL)
        return 0;

    fclose(file_ptr);

    return 1;
}

// Function to write formatted data to a file
// Return 1 on success, 0 on failure
int write_to_file(FILE *file_ptr, const char *fmt, ...)
{
    int result = 0;   
    va_list args;
    va_start(args, fmt);

    // Use vfprintf to write formatted data to the file
    // If the return value is non-negative, the write was successful
    if (vfprintf(file_ptr, fmt, args) >= 0)
    {
        result = 1; // Success
        fprintf(file_ptr, "\n"); // Add a newline character to the end of the line
    }
    va_end(args);
    
    return result;
}

// Function to read a line from the file and verify whether it matches the line given as a parameter.
// Return 1 on correct match, 0 when not
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
