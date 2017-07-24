#include "genLibrary.h"

// saveResults 3 -> read from 3W and save to 3out.txt

int main(int argc,char** argv){

    if (argc != 2) {
        perror("[ERROR]usage: ./saveResults <process id>");
        exit(-1);
    }

    int id = atoi(argv[1]);

    char writeFile[PIPE_NAME_SIZE];
    sprintf(writeFile,"temp/%dW",id);

    char buffer[PIPE_BUF]; cleanBuf(buffer,PIPE_BUF);

    int fp_fifo = open(writeFile,O_RDONLY);
    if (fp_fifo == -1) {
        errorOpeningFile();
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

    while (read(0,buffer,sizeof(buffer)) > 0) {

        if (write(1,buffer,strlen(buffer)) == -1 ) {
            errorWritingTerminal();
        }
    }

    return 0;
}
