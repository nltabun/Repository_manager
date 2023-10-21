#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "fileact.h"

#define ALIAS_MAX 130
#define LINK_MAX 256
#define LINE_MAX (ALIAS_MAX + LINK_MAX)
#define CMD_MAX 8
#define INPUT_MAX (CMD_MAX + LINE_MAX) // Should be atleast equivalent to highest max
#define FILE_NAME "repositories.csv"
#define HEADER "Alias,Link"
#define ENTRY_FMT "%s,%s"
#define ENTRY_SCAN_FMT "%[^,],%s"

enum CommandOperation
{
    QUIT = 1,
    ADD = 2,
    SHOW = 3,
    LIST = 4,
    DELETE = 5,
    HELP = 6,
    UNKNOWN = 0,
    TOO_FEW_ARGS = -1,
    TOO_MANY_ARGS = -2
};

typedef struct RepositoryEntry
{
    char alias[ALIAS_MAX];
    char link[LINK_MAX];
    struct RepositoryEntry *next;
} RepositoryEntry;

bool read_entry(FILE *file_ptr, char *alias, char *link);
bool add_new_entry(RepositoryEntry **head, const char *alias, const char *link);
bool delete_entry(RepositoryEntry **head, const char *alias);
int write_entries(FILE *file_ptr, RepositoryEntry *entry);
void free_list(RepositoryEntry *head);
void show_link(RepositoryEntry *head, const char *alias);
void print_all_aliases(RepositoryEntry *head);
void print_commands(void);
int parse_input(char *input, char *command, char *alias, char *link);
bool validate_arg_len(char *arg, int arg_type);
int validate_command(char *command, int arg_count);

int main(void)
{
    char user_input[INPUT_MAX];
    char n_alias[ALIAS_MAX];
    char n_link[LINK_MAX];
    char command[CMD_MAX];
    int command_args;
    int write_count;
    int execute_cmd;
    bool changes_saved;
    bool quit = false;
    FILE *file_ptr = NULL;
    RepositoryEntry *head = NULL;

    // Initialization
    // Check if file exists and if it doesn't, try to create it.
    // If the file exists then start reading entries from it
    if (!(does_file_exist(file_ptr, FILE_NAME)))
    {
        printf("File %s doesn't seem to exist. Attempting to create it..\n", FILE_NAME);
        if (create_file(file_ptr, FILE_NAME))
        {
            printf("Successfully created file %s\n", FILE_NAME);
            changes_saved = true;
        }
        else
        {
            fprintf(stderr, "Failed to create file %s. Can not continue. Exiting..\n", FILE_NAME);
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("File %s found.\n", FILE_NAME);
        changes_saved = true;

        file_ptr = fopen(FILE_NAME, "r");
        if (file_ptr == NULL)
        {
            fprintf(stderr, "Failed to open file %s for reading entries. Existing entries could be lost if you attempt to add/remove entries.\n", FILE_NAME);
        }
        else
        {
            // Check if header exists
            if (!(verify_line_from_file(file_ptr, HEADER, LINE_MAX)))
            {
                fprintf(stderr, "Missing correct header.\n");
                changes_saved = false; // So that header gets added even if no new entries get added/removed.
                rewind(file_ptr); // First line wasn't a header so back to it
            }

            // Read entries from file and populate list
            while (!feof(file_ptr))
            {
                if (read_entry(file_ptr, n_alias, n_link))
                {
                    if (!(add_new_entry(&head, n_alias, n_link)))
                        fprintf(stderr, "Failed to add entry\n");
                }
            }

            fclose(file_ptr);
        }
    }
    // Init ends

    printf("\nREPOSITORY MANAGER\nType \"help\" to list available commands\n");
    // Main program loop
    while (!quit)
    {
        printf("> ");
        // Get user input
        if (fgets(user_input, sizeof(user_input), stdin) != NULL)
        {
            memset(command, 0, sizeof(command)); // Clear command string

            // Parse input and extract command, alias and link
            command_args = parse_input(user_input, command, n_alias, n_link);

            if (command_args >= 0)
            {
                // Validate command and execute corresponding operation
                execute_cmd = validate_command(command, command_args);

                switch (execute_cmd)
                {
                case QUIT:
                    // If there are unsaved changes then write them to the repository file before ending the program
                    if (!changes_saved)
                    {
                        file_ptr = fopen(FILE_NAME, "w");
                        if (file_ptr == NULL)
                            fprintf(stderr, "Failed to open file for writing. Unable to save changes.\n");
                        else
                        {
                            if (write_to_file(file_ptr, HEADER) != 1)
                                fprintf(stderr, "Failed to write header to file.\n");

                            write_count = write_entries(file_ptr, head);
                            printf("Wrote %d entries to file.\n", write_count);

                            fclose(file_ptr);
                        }
                    }

                    printf("Quitting..\n");
                    quit = true; // Main program loop over
                    break;
                case ADD:
                    if (!(add_new_entry(&head, n_alias, n_link)))
                        fprintf(stderr, "Failed to add entry\n");
                    else
                    {
                        printf("Successfully added entry.\n");
                        changes_saved = false;
                    }
                    break;
                case SHOW:
                    show_link(head, n_alias);
                    break;
                case LIST:
                    print_all_aliases(head);
                    break;
                case DELETE:
                    if (delete_entry(&head, n_alias))
                    {
                        printf("Deleted: %s\n", n_alias);
                        changes_saved = false;
                    }
                    else
                        printf("Did not find \"%s\". Make sure your spelling is correct.\n", n_alias);
                    break;
                case HELP:
                    print_commands();
                    break;
                case UNKNOWN:
                    printf("Unknown command. Type \"help\" to list available commands\n");
                    break;
                case TOO_FEW_ARGS:
                    printf("Too few arguments for command \"%s\".\n", command);
                    break;
                case TOO_MANY_ARGS:
                    printf("Too many arguments for command \"%s\".\n", command);
                    break;
                default:
                    // This should not be possible under normal circumstances
                    fprintf(stderr, "Invalid command operation encountered. Command validation failed.\n");
                    break;
                }
            }
        }
    }

    // Free memory allocated for the list
    free_list(head);

    return 0;
}

// Function to read an entry from the file 
bool read_entry(FILE *file_ptr, char *alias, char *link)
{
    char read_line[LINE_MAX];

    // Read a line from the file
    if (fgets(read_line, sizeof(read_line), file_ptr) == NULL)
        return false;

    // Parse the line to extract alias and link
    if (sscanf(read_line, ENTRY_SCAN_FMT, alias, link) != 2)
        return false;

    return true;
}

// Function to add a new entry to the linked list
bool add_new_entry(RepositoryEntry **head, const char *alias, const char *link)
{
    // Try to create a new entry
    RepositoryEntry *new_entry = (RepositoryEntry *)malloc(sizeof(RepositoryEntry));
    if (!new_entry)
    {
        fprintf(stderr, "Memory allocation failed. Could not add new entry\n");
        return false;
    }

    // Fill entry with data
    strcpy(new_entry->alias, alias);
    strcpy(new_entry->link, link);

    // Add entry to the end of the linked list
    if (*head == NULL)
    {
        // If the list is empty, set the new entry as the head
        *head = new_entry;
        new_entry->next = NULL;
    }
    else
    {
        // Find the end of the list and append the new entry
        while (*head != NULL)
            head = &(*head)->next;

        *head = new_entry;
        new_entry->next = NULL;
    }

    return true;
}


// Function to delete an entry from the linked list
// Only deletes the first one (oldest)
bool delete_entry(RepositoryEntry **head, const char *alias)
{
    if (!head || !(*head))
        return false;

    RepositoryEntry *temp = *head;
    RepositoryEntry *prev = NULL;

    // Traverse the list to find the entry with given alias
    while (strcmp(temp->alias, alias) != 0 && temp->next != NULL)
    {
        prev = temp;
        temp = temp->next;
    }

    // If the entry with the given alias is found
    if (strcmp(temp->alias, alias) == 0)
    {
        if (prev)
            prev->next = temp->next;
        else
            *head = temp->next;

        free(temp);

        return true;
    }

    return false; // Entry with given alias not found
}

// Function to write entries from the linked list to the file
int write_entries(FILE *file_ptr, RepositoryEntry *entry)
{
    RepositoryEntry *current = entry;
    int count = 0;

    // Traverse the linked list
    while (current != NULL)
    {
        // Write the entry to the file using the correct format
        if (write_to_file(file_ptr, ENTRY_FMT, current->alias, current->link) == 1)
            count++; // Increment count if write successful
        else
            fprintf(stderr, "Failed to write entry to file.\n");

        current = current->next;
    }

    return count; // Return the count of successfully written entries
}

// Function to free memory allocated for the linked list
void free_list(RepositoryEntry *head)
{
    RepositoryEntry *current = head;
    // Traverse the linked list
    while (current != NULL)
    {
        RepositoryEntry *temp = current;
        current = current->next;
        free(temp); // Free memory for each entry node in the linked list
    }
}

// Function to display the link(s) associated with an alias or all links
void show_link(RepositoryEntry *head, const char *alias)
{
    RepositoryEntry *current = head;
    bool show_all = false;
    bool found = false;

    // Check if the repository list is empty and return early if so.
    if (current == NULL)
    {
        printf("Repository list is empty.\n");
        return;
    }

    // Check if the user wants to show links for all aliases or a specific one.
    if (strcmp("all", alias) == 0)
    {
        show_all = true;
        found = true;
        printf("Showing links for all aliases:\n");
    }
    else
        printf("Showing link(s) for %s:\n", alias);

    // Traverse the linked list and print link(s) that match provided alias
    while (current != NULL)
    {
        if (show_all)
            printf("%s: %s\n", current->alias, current->link);
        else if (strcmp(current->alias, alias) == 0)
        {
            printf("%s: %s\n", current->alias, current->link);
            found = true;
        }

        current = current->next;
    }

    // If the alias was not found
    if (!found)
        printf("Could not find \"%s\". Make sure your spelling is correct.\n", alias);
}

// Function to print all aliases in the linked list
void print_all_aliases(RepositoryEntry *head)
{
    RepositoryEntry *current = head;
    printf("Showing all aliases:\n");
    while (current != NULL)
    {
        printf("- %s\n", current->alias);
        current = current->next;
    }
}

// Function to print all available commands
void print_commands(void)
{
    printf(
        "Available commands:\n"
        "- add <alias> <repository link>\n"
        "- show <alias>|all\n"
        "- list\n"
        "- delete <alias>\n"
        "- help\n"
        "- quit\n");
}

// Function to parse user input and extract command, alias and link
int parse_input(char *input, char *command, char *alias, char *link)
{
    int arg_count = -1;
    char *input_ptr;

    // Check for input length exceeding the limit
    if (strchr(input, '\n') == NULL)
    {
        fprintf(stderr, "Input contains too many characters.\n");
        while (getchar() != '\n');
        return arg_count;
    }

    input[strcspn(input, "\n")] = 0;
    input_ptr = strtok(input, " ");

    if (input_ptr != NULL)
    {
        while (input_ptr)
        {
            switch (arg_count)
            {
            case -1:
                // Validate and extract command
                if (validate_arg_len(input_ptr, arg_count))
                {
                    arg_count++;
                    strcpy(command, input_ptr);
                    break;
                }
                else
                {
                    fprintf(stderr, "Command contains too many characters. Check commands with \"help\".\n");
                    return -1;
                }
            case 0:
                // Validate and extract alias
                if (validate_arg_len(input_ptr, arg_count))
                {
                    arg_count++;
                    strcpy(alias, input_ptr);
                    break;
                }
                else
                {
                    fprintf(stderr, "Alias contains too many characters. (Max: %d)\n", ALIAS_MAX - 1);
                    return -1;
                }
            case 1:
                // Validate and extract link
                if (validate_arg_len(input_ptr, arg_count))
                {
                    arg_count++;
                    strcpy(link, input_ptr);
                    break;
                }
                else
                {
                    fprintf(stderr, "Link contains too many characters. (Max: %d)\n", LINK_MAX - 1);
                    return -1;
                }
            default:
                return 3;
            }

            input_ptr = strtok(NULL, " ");
        }
    }

    return arg_count;
}

// Function to validate argument length
// Return false if too long
bool validate_arg_len(char *arg, int arg_type)
{
    switch (arg_type)
    {
    case -1: // Command
        if (strlen(arg) >= CMD_MAX)
            return false;
        break;
    case 0: // Alias
        if (strlen(arg) >= ALIAS_MAX)
            return false;
        break;
    case 1: // Link
        if (strlen(arg) >= LINK_MAX)
            return false;
        break;
    default:
        return false;
    }

    return true;
}

// Function to validate the command and its argument count
// Return value for validated command or error message to run
int validate_command(char *command, int arg_count)
{
    if (strcmp(command, "quit") == 0)
    {
        if (arg_count == 0)
            return QUIT;
        else
            return TOO_MANY_ARGS;
    }
    else if (strcmp(command, "add") == 0)
    {
        if (arg_count == 2)
            return ADD;
        else if (arg_count < 2)
            return TOO_FEW_ARGS;
        else
            return TOO_MANY_ARGS;
    }
    else if (strcmp(command, "show") == 0)
    {
        if (arg_count == 1)
            return SHOW;
        else if (arg_count < 1)
            return TOO_FEW_ARGS;
        else
            return TOO_MANY_ARGS;
    }
    else if (strcmp(command, "list") == 0)
    {
        if (arg_count == 0)
            return LIST;
        else
            return TOO_MANY_ARGS;
    }
    else if (strcmp(command, "delete") == 0)
    {
        if (arg_count == 1)
            return DELETE;
        else if (arg_count < 2)
            return TOO_FEW_ARGS;
        else
            return TOO_MANY_ARGS;
    }
    else if (strcmp(command, "help") == 0)
    {
        if (arg_count == 0)
            return HELP;
        else
            return TOO_MANY_ARGS;
    }
    else
        return UNKNOWN;
}
