#include <stdio.h>
#include <stdbool.h>

#define ALIAS_MAX 130
#define LINK_MAX 256
#define FILE_NAME_MAX 96

typedef struct repository
{
    char alias[ALIAS_MAX];
    char link[LINK_MAX];
    struct repository* next;
} nrepository;

bool does_file_exist(FILE *file_ptr, const char *file_name);
bool create_repository_list_file(FILE *file_ptr, const char *file_name);

int main(int argc, char const *argv[])
{
    FILE *file_ptr;
    char file_name[FILE_NAME_MAX] = "repositories.csv";

    // Check if file exists and if it doesn't, try to create it
    if (!(does_file_exist(file_ptr, file_name)))
    {
        printf("File %s doesn't seem to exist. Attempting to create it..\n", file_name);
        if (create_repository_list_file(file_ptr, file_name))
        {
            printf("Successfully created file %s\n", file_name);
        }
        else
        {
            printf("Failed to create file %s. Can not continue. Exiting..\n", file_name);
            return 3;
        }

    }
    else
    {
        printf("File %s found.\n", file_name);
    }

    return 0;
}

bool does_file_exist(FILE *file_ptr, const char *file_name)
{
    file_ptr = fopen(file_name, "r");

    if (file_ptr == NULL)
        return false;

    fclose(file_ptr);
    
    return true;
}

bool create_repository_list_file(FILE *file_ptr, const char *file_name)
{
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL)
        return false;    

    fclose(file_ptr);

    return true;
}