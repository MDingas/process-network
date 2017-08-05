#include "genLibrary.h"

int main(int argc, char** argv){
    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);

    while(1){
        while(readln(0,buffer,sizeof(buffer)) > 0){
            remove_newline(buffer);
            if(prefix_match("M,",buffer) == 1){
                add_newline(buffer);
                if(write(1,buffer,strlen(buffer)) == -1){ error_writing_terminal(); }
                clean_buffer(buffer,sizeof(buffer));
            }

        }
    }
}
