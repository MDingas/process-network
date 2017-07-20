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

void safePrintf(char* text);
ssize_t readln(int fd, char* buffer, long maxBytes);
int prefixMatch(char* prefix,char* word);
void cleanBuf(char* buf,int size);
void removeNewline(char* buf);
void addNewline(char* bufer);
char* longToString(long num,long size); /* convert long to string */
char** splitAt(char* string,char delimiter); /* like Haskell's splitAt */
int eventNums(char** events);
long getColumn(char* event,long column); /* what's the nr at that column */
char* unsplitAs(char** strings,char delimiter);
void constEvent(char* event,char* const constval); /* add number to single event */
