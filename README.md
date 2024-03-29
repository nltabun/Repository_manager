# Repository manager
Program for keeping track repositories you use. The program records aliases (short names)
for long repository links and allows user to view and manage the aliases.

Available commands are:
* add \<alias> \<repository link>
  * add a new entry (multiple entries with the same alias are allowed)
* show \<alias> | all
  * show the repository link(s) of a given alias or show all links
* list
  * prints a list of all aliases
* quit
  * ends the program (and saves changes if the list was modified)
* delete <alias>
  * deletes the given entry (only the oldest one if multiple of the same alias exist)
* help
  * prints a list of available commands

Repositories get saved to file called "repositories.csv"