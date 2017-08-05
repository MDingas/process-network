#include "genLibrary.h"

int main(int argc, char** argv){
    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);
    if (argc < 1) {
        perror("usage: ./constainsWord <string to filter>\n");
        exit(-1);
    }

    char keyword[200]; clean_buffer(keyword,200);

    for (int i = 1; i < argc; i++) {
        strcat(keyword,argv[i]);
        if (i != argc - 1){
            strcat(keyword," ");
        }
    }

    while(1){
        while(readln(0,buffer,sizeof(buffer)) > 0){
            remove_newline(buffer);
            if(strstr(buffer,keyword)){
                add_newline(buffer);
                if(write(1,buffer,strlen(buffer)) == -1){ error_writing_terminal(); }
                clean_buffer(buffer,sizeof(buffer));
            }

        }
    }
}
