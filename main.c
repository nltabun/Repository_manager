#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define ALIAS_MAX 130
#define LINK_MAX 256
#define LINE_MAX (ALIAS_MAX + LINK_MAX + 1)
#define FILE_NAME_MAX 96

typedef struct RepositoryEntry
{
    char alias[ALIAS_MAX];
    char link[LINK_MAX];
    struct RepositoryEntry* next;
} RepositoryEntry;

// TODO: maybe move these 3 to another file
bool does_file_exist(FILE *file_ptr, const char *file_name);
bool create_file(FILE *file_ptr, const char *file_name);
bool write_to_file(FILE *file_ptr, const char *fmt, ...);

bool add_new_entry(RepositoryEntry **head, const char *alias, const char *link);
void show_link(RepositoryEntry *head, const char *alias);
void print_all_aliases(RepositoryEntry *head);
void delete_entry(RepositoryEntry *head, const char *alias);
void free_list(RepositoryEntry *head);
void print_commands();


int main(int argc, char const *argv[])
{
    FILE *file_ptr;
    char file_name[FILE_NAME_MAX] = "repositories.csv";
    const char format[] = "%s,%s\n";
    RepositoryEntry *head = NULL;
    char new_alias[ALIAS_MAX] = "GitHub";
    char new_link[LINK_MAX] = "https://github.com";

    printf("LINE MAX: %d\n", LINE_MAX);

    // Check if file exists and if it doesn't, try to create it
    if (!(does_file_exist(file_ptr, file_name)))
    {
        printf("File %s doesn't seem to exist. Attempting to create it..\n", file_name);
        if (create_file(file_ptr, file_name))
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
        printf("File %s found.\n", file_name);


    // Test add
    if (add_new_entry(&head, new_alias, new_link))
    {
        printf("Successfully added new entry: %s\n", new_alias);
    }
    else
    {
        printf("Failed to add new entry: %s\n", new_alias);
    }

    // Test write to file
    file_ptr = fopen(file_name, "w");
    if (file_ptr == NULL)
    {
        printf("Failed to open file for writing.\n");
        return 4;
    }
    
    if (write_to_file(file_ptr, format, new_alias, new_link))
    {
        printf("Successfully wrote entry to file.\n");
    }
    else
    {
        printf("Failed to write entry to file.\n");
    }

    fclose(file_ptr);
    

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

bool create_file(FILE *file_ptr, const char *file_name)
{
    file_ptr = fopen(file_name, "w");

    if (file_ptr == NULL)
        return false;    

    fclose(file_ptr);

    return true;
}

bool write_to_file(FILE *file_ptr, const char *fmt, ...)
{
    bool result = false;    
    va_list args;
    va_start(args, fmt);
    if (vfprintf(file_ptr, fmt, args) >= 0)
    {
        result = true;
    }
    va_end(args);
    
    return result;
}

bool add_new_entry(RepositoryEntry **head, const char *alias, const char *link)
{
    // Try to create a new entry
    RepositoryEntry *new_entry = (RepositoryEntry *)malloc(sizeof(RepositoryEntry));
    if (!new_entry)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        return false;
    }
    
    // Fill entry with data
    strcpy(new_entry->alias, alias);
    strcpy(new_entry->link, link);

    // Add entry to the end of the linked list
    if (*head == NULL)
    {
        *head = new_entry;
        new_entry->next = NULL;
    }
    else
    {
        while (*head != NULL)
        {
            head = &(*head)->next;
        }
        *head = new_entry;
        new_entry->next = NULL;
    }
    
    return true;
}


// Clears list / frees memory
void free_list(RepositoryEntry *head)
{
    RepositoryEntry *current = head;
    while (current != NULL)
    {
        RepositoryEntry *temp = current;
        current = current->next;
        free(temp);
    }
}