#include "genLibrary.h"

/*
 * Append a constant value to a string
 * For example
 * ./const 10
 * a:b:c -> a:b:c:10
 */

int main(int argc,char** argv){
    if(argc < 2){
        perror("usage: ./const <value to append>");
        exit(-1);
    }

    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);
    char constValue[200]; clean_buffer(constValue,200);

    for (int i = 1; i < argc; i++) {
        strcat(constValue,argv[i]);
        if (i != argc - 1){
            strcat(constValue," ");
        }
    }

    while(1){
        while(readln(0,buffer,sizeof(buffer)) > 0){
            remove_newline(buffer);
            strcat(buffer,":");
            strcat(buffer,constValue);
            add_newline(buffer);
            if(write(1,buffer,strlen(buffer)) == -1){
                error_writing_terminal();
            }
            clean_buffer(buffer,sizeof(buffer));
        }
    }

    return 0;
}
