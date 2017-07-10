#include "genLibrary.h" /* filterEvent() cleanBuf() */

int filterEvent(long column,char* operator,long value,char* event){

    long eventVal = getColumn(event,column);

    if(!strcmp(operator,"=")){
        return( (eventVal == value) ? 1 : 0);

    }else if (!strcmp(operator,">=")){
        return( (eventVal >= value) ? 1 : 0);

    }else if (!strcmp(operator,"<=")){
        return( (eventVal <= value) ? 1 : 0);

    }else if (!strcmp(operator,"<")){
        return( (eventVal < value) ? 1 : 0);

    }else if (!strcmp(operator,">")){
        return( (eventVal > value) ? 1 : 0);

    }else if (!strcmp(operator,"!=")){
        return( (eventVal != value) ? 1 : 0);

    }else{
        perror("[ERROR]undefined operator");
        return 0;
    }
}

/*Pass the events through only if it passes a premisse
 *
 * filter <column> <comparator> <value> *
 * filter 2 >= 3 1:2:3:4  -> (empty)
 * filter 2 < 3  1:2:3:4  -> 1:2:3:4
 */

int main(int argc,char** argv){
    if(argc != 4){

        fprintf(stderr,"Usage: ./filter <column> < < / > / != / = / >= / <= > <value>");
        exit(-1);
    }

    long column = atol(argv[1]);
    char* operation = argv[2];
    long value = atol(argv[3]);

    char buffer[PIPE_BUF];

    while(1){

        int status;
        cleanBuf(buffer,PIPE_BUF);

        status = read(0,buffer,sizeof(buffer));
        if(status == -1) {
            fprintf(stderr,"Error reading"); exit(-1);}
        if(status == 0) exit(0);

        if(filterEvent(column,operation,value,buffer) == 1){
            strcat(buffer,"\n");
            if(write(1,buffer,strlen(buffer)) == -1){ errorWritingTerminal(); }
            cleanBuf(buffer,PIPE_BUF);
        }
    }

    return 0;
}
