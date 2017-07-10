#include "genLibrary.h"

int main(int argc,char** argv){
      if(argc != 2){
            perror("usage: ./const <value to append>");
            exit(-1);
      }

      char buffer[PIPE_BUF]; cleanBuf(buffer,PIPE_BUF);

      while(1){
            while(readln(0,buffer,sizeof(buffer)) > 0){
                  removeNewline(buffer);
                  strcat(buffer,":");
                  strcat(buffer,argv[1]);
                  strcat(buffer,"\n");
                  write(1,buffer,strlen(buffer));
                  cleanBuf(buffer,sizeof(buffer));
            }
      }

      return 0;
}
