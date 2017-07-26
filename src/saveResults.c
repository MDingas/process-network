#include "genLibrary.h"

/*
 * When called, it saves all output of a node into a permanent file
 * Essentially, it reads from the nodes write pipe and saves into a permanent file in the output directory
 *
 *
 */

int main(int argc,char** argv){

    if (argc != 2) {
        perror("[ERROR]usage: ./saveResults <process id>");
        exit(-1);
    }

    int id = atoi(argv[1]);

    char writeFile[PIPE_NAME_SIZE];
    sprintf(writeFile,"temp/%dW",id);

    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);

    int fp_fifo = open(writeFile,O_RDONLY);
    if (fp_fifo == -1) {
        error_opening_file();
    }

    char storeFile[PIPE_NAME_SIZE];
    sprintf(storeFile,"output/%dout",id);

    int fp_create = open(storeFile, O_CREAT,0666);
    if( fp_create == -1){
        perror("[ERROR]creating fifo");
        exit(-1);
    }

    int fp_store = open(storeFile, O_APPEND | O_WRONLY);
    if (fp_store == -1) {
        perror("[ERROR]reading or creating fifo");
        exit(-1);
    }

    dup2(fp_fifo,0); close(fp_fifo);
    dup2(fp_store,1); close(fp_store);

    if (read(0,buffer,sizeof(buffer)) == -1) {
         error_reading_terminal();
    }

    if (write(1,buffer,strlen(buffer)) == -1) {
        error_writing_terminal();
    }

    return 0;
}
