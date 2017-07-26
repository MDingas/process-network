#include "genLibrary.h" /* filterEvent() clean_buffer() */

int filterEvent(long column,char* operator,long value,char* event){

    long eventVal = get_column(event,column);

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

/*
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

    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);
    while(1){
        while(readln(0,buffer,sizeof(buffer)) > 0){
            remove_newline(buffer);
            if(filterEvent(column,operation,value,buffer) == 1){
                add_newline(buffer);
                if(write(1,buffer,strlen(buffer)) == -1){ error_writing_terminal(); }
                clean_buffer(buffer,sizeof(buffer));
            }

        }
    }

    return 0;
}
