# Repository manager
Write a program for keeping track repositories you use. The program records aliases (short names)
for long repository links and allows user to view and mange the aliases.
The programs ask user what to do and parses the user input for supported commands. The
commands are:
* add \<alias> \<repository link>
  * adds a new entry
* show \<alias> | all
  * user can show the repository link of a given alias or show all links
* list
  * prints a list of all aliases
* quit
  * ends the program
* delete <alias>
  * deletes the given entry
* help
  * prints a list of available commands

If the command is not recognized an error message must be printed.
Program must read the information from a file when program starts and must save the changes to
the file either immediately after change or when program ends.