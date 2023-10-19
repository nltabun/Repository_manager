#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "fileact.h"

#define ALIAS_MAX 130
#define LINK_MAX 256
#define LINE_MAX (ALIAS_MAX + LINK_MAX)
#define FILE_NAME_MAX 96
#define CMD_MAX 8
#define INPUT_MAX LINK_MAX // Should be atleast equivalent to highest max
#define DEFAULT_FILE_NAME "repositories.csv"
#define HEADER "Alias,Link"
#define ENTRY_FMT "%s,%s"
#define ENTRY_SCAN_FMT "%[^,],%s"
#define CMD_FMT "%s %s %s %s"

typedef struct RepositoryEntry
{
    char alias[ALIAS_MAX];
    char link[LINK_MAX];
    struct RepositoryEntry *next;
} RepositoryEntry;

bool init_program(FILE file_ptr, const char *file_name, bool *saved);
bool read_entry(FILE *file_ptr, const char *alias, const char *link);
bool add_new_entry(RepositoryEntry **head, const char *alias, const char *link);
int write_entries(FILE *file_ptr, RepositoryEntry *entry);
void show_link(RepositoryEntry *head, const char *alias);
void print_all_aliases(RepositoryEntry *head);
void delete_entry(RepositoryEntry *head, const char *alias);
void free_list(RepositoryEntry *head);
void print_commands();
bool test(FILE *file_ptr, RepositoryEntry *head, const char *file_name, char *user_alias, char *user_link);

int main(int argc, char const *argv[])
{
    FILE *file_ptr;
    char file_name[FILE_NAME_MAX] = DEFAULT_FILE_NAME;
    char line[LINE_MAX];
    bool quit = false;
    RepositoryEntry *head = NULL;
    char user_input[INPUT_MAX];
    char user_alias[ALIAS_MAX];
    char user_link[LINK_MAX];
    char command[CMD_MAX];
    int command_args;
    bool changes_saved;

    // Init starts
    // Check if file exists and if it doesn't, try to create it
    if (!(does_file_exist(file_ptr, file_name)))
    {
        printf("File %s doesn't seem to exist. Attempting to create it..\n", file_name);
        if (create_file(file_ptr, file_name, ENTRY_FMT))
        {
            printf("Successfully created file %s\n", file_name);
            changes_saved = true;
        }
        else
        {
            printf("Failed to create file %s. Can not continue. Exiting..\n", file_name);
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("File %s found.\n", file_name);
        changes_saved = true;
    }

    file_ptr = fopen(file_name, "r");

    // Check header TODO: Handle somehow
    if (!(verify_line_from_file(file_ptr, HEADER, LINE_MAX)))
    {
        printf("Missing correct header.\n");
    }

    // Read entries from file and populate list
    while (!feof(file_ptr))
    {
        if (read_entry(file_ptr, user_alias, user_link))
        {
            if (!(add_new_entry(&head, user_alias, user_link)))
            {
                printf("Failed to add entry\n");
            }
        }
    }

    fclose(file_ptr);
    // Init ends

    printf("Welcome to REPOSITORY MANAGER\nType \"help\" to list available commands\n");
    // Main program loop
    while (!quit)
    {
        printf(">");
        if (fgets(user_input, sizeof(user_input), stdin) != NULL) // TODO: validation in function
        {
            memset(command, 0, sizeof(command)); // Clear command string
            user_input[strcspn(user_input, "\n")] = 0;
            command_args = sscanf(user_input, CMD_FMT, command, user_alias, user_link);
            printf("Args: %d\n", command_args);
            printf("Cmd: %s, Alias: %s, Link: %s\n", command, user_alias, user_link);

            if (strcmp(command, "quit") == 0)
            {
                if (!changes_saved)
                {
                    file_ptr = fopen(file_name, "w");
                    if (file_ptr == NULL)
                    {
                        printf("Failed to open file for writing.\n");
                        return EXIT_FAILURE;
                    }

                    if (write_to_file(file_ptr, HEADER) == 1)
                    {
                        printf("Successfully wrote header to file.\n");
                    }
                    else
                    {
                        printf("Failed to write header to file.\n");
                    }

                    int entry_w = write_entries(file_ptr, head);
                    printf("Wrote %d entries to file.\n", entry_w);

                    fclose(file_ptr);
                }

                printf("Quitting..\n");
                quit = true;
            }
            else if (strcmp(command, "add") == 0)
            {
                if (command_args == 3)
                {
                    if (!(add_new_entry(&head, user_alias, user_link)))
                    {
                        printf("Failed to add entry\n");
                    }
                    else
                    {
                        printf("Successfully added entry.\n");
                        changes_saved = false;
                    }
                }
                else
                {
                    printf("Missing required arguments for command \"add\".\n");
                }
            }
            else if (strcmp(command, "show") == 0)
            {
                /* code */
            }
            else if (strcmp(command, "list") == 0)
            {
                print_all_aliases(head);
            }
            else if (strcmp(command, "delete") == 0)
            {
                /* code */
            }
            else if (strcmp(command, "help") == 0)
            {
                /* code */
            }
            else
            {
                printf("Unknown command. Type \"help\" to list available commands\n");
            }
        }
    }

    // Run test
    // test(file_ptr, head, file_name, user_alias, user_link);

    // Free memory
    free_list(head);

    return 0;
}

bool init_program(FILE file_ptr, const char *file_name, bool *saved)
{

    return true;
}
// Read entry from file and add it to list
// TODO: add rigorous checking for invalid readings.
bool read_entry(FILE *file_ptr, const char *alias, const char *link)
{
    char read_line[LINE_MAX];

    if (fgets(read_line, sizeof(read_line), file_ptr) == NULL)
        return false;

    if (sscanf(read_line, ENTRY_SCAN_FMT, alias, link) != 2)
        return false;

    return true;
}

int write_entries(FILE *file_ptr, RepositoryEntry *entry)
{
    RepositoryEntry *current = entry;
    int count = 0;

    while (current != NULL)
    {
        if (write_to_file(file_ptr, ENTRY_FMT, current->alias, current->link) == 1)
        {
            count++;
        }
        else
        {
            printf("Failed to write entry to file.\n");
        }
        current = current->next;
    }

    return count;
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

void print_all_aliases(RepositoryEntry *head)
{
    RepositoryEntry *current = head;
    printf("SHOWING ALL ALIASES\n");
    while (current != NULL)
    {
        printf("- %s\n", current->alias);
        current = current->next;
    }
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

bool test(FILE *file_ptr, RepositoryEntry *head, const char *file_name, char *user_alias, char *user_link)
{
    // Test print aliases
    print_all_aliases(head);

    // Test add
    if (add_new_entry(&head, user_alias, user_link))
    {
        printf("Successfully added new entry: %s|%s\n", user_alias, user_link);
    }
    else
    {
        printf("Failed to add new entry: %s|%s\n", user_alias, user_link);
    }

    // Test print aliases
    print_all_aliases(head);

    // Test write to file
    file_ptr = fopen(file_name, "w");
    if (file_ptr == NULL)
    {
        printf("Failed to open file for writing.\n");
        return false;
    }

    // Test write back to file
    if (write_to_file(file_ptr, HEADER) == 1)
    {
        printf("Successfully wrote header to file.\n");
    }
    else
    {
        printf("Failed to write to file.\n");
    }

    int entry_w = write_entries(file_ptr, head);
    printf("Wrote %d entries to file.\n", entry_w);

    fclose(file_ptr);

    // Test print aliases
    print_all_aliases(head);

    return true;
}