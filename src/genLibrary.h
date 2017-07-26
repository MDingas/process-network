#include <string.h> /* strcat(), strlen(), strcmp() */
#include <fcntl.h> /* open(), O_RDONLY, O_WRONLY */
#include <unistd.h> /* read(), write() */
#include <sys/wait.h> /* waitpid() */
#include <sys/stat.h> /* mkfifo() */
#include <limits.h> /* PIPE_BUF */
#include <stdio.h> /* fprintf() */
#include <stdlib.h> /* exit() */
#include "errors.h"

#define PIPE_NAME_SIZE 15 /* How long the name of a fifo can be */
#define NUM_DIGITS 50 /* Max number of digits in a single number of an event */
#define MAX_IDS 100 /* Maximum number of ids in an array of nodes and matrix of node connections */

void safe_printf(char* text);
ssize_t readln(int fd, char* buffer, long maxBytes);
int prefix_match(char* prefix,char* word);
void clean_buffer(char* buf,int size);
void remove_newline(char* buf);
void add_newline(char* bufer);
char* long_to_string(long num,long size); /* convert long to string */
char** split_with_delimiter(char* string,char delimiter);
int event_number(char** events);
long get_column(char* event,long column); /* what's the nr at that column */
char* merge_strings(char** strings,char delimiter);
void append_value(char* event,char* const constval); /* add number to single event */
