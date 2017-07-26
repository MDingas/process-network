#include "genLibrary.h"

void error_reading_terminal(){
    perror("[ERROR]reading from terminal");
    exit(-1);
}

void error_writing_terminal(){
    perror("[ERROR]writing to terminal");
    exit(-1);
}

void error_executing(){
    perror("[ERROR]executing process");
    exit(-1);
}

void error_mkfifo(){
    perror("[ERROR]creating fifo");
    exit(-1);
}

void error_forking(){
    perror("[ERROR]forking process");
    exit(-1);
}

void error_pipe(){
    perror("[ERROR]creating pipe");
    exit(-1);
}

void error_opening_file(){
    perror("[ERROR]opening file");
    exit(-1);
}
