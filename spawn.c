#include "genLibrary.h" /* cleanBuf() constEvent() removeNewline() */

/* Execute a command and append return status to end of line
 *
 * echo -n ola 1:2:3 -> ola1:2:3:0
 */

int main(int argc,char** argv){
    if(argc < 2){
        perror("usage: ./spawn < unix command >");
        exit(-1);
    }

    while(1){
        char* commandArgs[argc];
        for(int i = 1, j = 0; i < argc; i++,j++)
            commandArgs[j] = argv[i];
        commandArgs[argc] = NULL;

        char buffer[PIPE_BUF];
        cleanBuf(buffer,PIPE_BUF);

        if(read(0,buffer,sizeof(buffer)) == -1){ errorReadingTerminal(); }
        if(strlen(buffer) != 0){ /* no input and newLine */
            removeNewline(buffer);

            /* Execute command with forked process */
            pid_t pd;

            if( (pd = fork()) == -1){ errorForking(); }

            /* son process */
            if(pd == 0){ execvp(commandArgs[0],commandArgs); }

            /* wait for son process to finish, save exit status on variable status */
            int status;
            waitpid(pd,&status,0);

            char* stringStatus = longToString(status,NUM_DIGITS);

            /* reuse the const function */
            constEvent(buffer,stringStatus);
            strcat(buffer,"\n");

            if(write(1,buffer,strlen(buffer)) == -1){ errorWritingTerminal(); }
            cleanBuf(buffer,sizeof(buffer));
        }
    }

    return 0;
}
