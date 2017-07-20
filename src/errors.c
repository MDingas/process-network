#include "genLibrary.h"

void errorReadingTerminal(){
    perror("[ERROR]reading from terminal");
    exit(-1);
}

void errorWritingTerminal(){
    perror("[ERROR]writing to terminal");
    exit(-1);
}

void errorExecuting(){
    perror("[ERROR]executing process");
    exit(-1);
}

void errorMkfifo(){
    perror("[ERROR]creating fifo");
    exit(-1);
}

void errorForking(){
    perror("[ERROR]forking process");
    exit(-1);
}

void errorPipe(){
    perror("[ERROR]creating pipe");
    exit(-1);
}

void errorOpeningFile(){
    perror("[ERROR]opening file");
    exit(-1);
}
