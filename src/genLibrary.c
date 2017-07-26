#include "genLibrary.h"

/* SAFE PRINTF
 *
 * since printf is unsafe when using read() and write() system calls, this was needed
 *
 */
void safe_printf(char* text){
    if( write(1,text,strlen(text)) == -1){
        perror("[ERROR]writing to terminal\n");
        exit(-1);
    }
}

/* PREFIX MATCH
 *
 * Used for parsing user input, do two words share the same prefix?
 *
 */
int prefix_match(char* prefix,char* word){

    int i = 0;
    if(!word)
        return 0;
    while(prefix[i] != '\0' && word[i] != '\0'){

        if(prefix[i] != word[i])
            return 0;
        i++;
    }
    if(prefix[i] != '\0') // otherwise prefix_match("Diana","D") is a-ok and we don't want that
        return 0;
    return 1;
}

/* CLEAN BUFFER
 *
 * Fill a buffer with '\0'
 *
 */
void clean_buffer(char* buf,int size){
    for(int i = 0; i < size ; i++)
        buf[i] = '\0';
}

void remove_newline(char* buf){
    for(int i = 0; buf[i] != '\0'; i++)
        if(buf[i] == '\n')
            buf[i] = '\0';
}

void add_newline(char* buf){
      strcat(buf,"\n");
}

/* Split a word by a certain delimiter
 * Currently the function wastes a lot of memory and isn't very efficient
 *
 * f("test:one:two",':') -> {"test","one","two",NULL}
 *
 */
char** split_with_delimiter(char* string, char delimiter){

    char** output = malloc(sizeof(char*) * PIPE_BUF);
    for(int i = 0; string[i] != '\0'; i++){

        output[i] = malloc(sizeof(char) * PIPE_BUF);
        clean_buffer(output[i],PIPE_BUF);
    }

    int j = 0;
    char* buffer = malloc(sizeof(char) * PIPE_BUF);

    for(int i = 0; string[i] != '\0'; i++){


        if(string[i] != delimiter){


            buffer[0] = string[i];
            buffer[1] = '\0';
            strcat(output[j],buffer);
        }
        else{


            j++;
        }
    }


    free(buffer);
    output[j+1] = NULL;
    return output;
}

int getBytes(char** strings){

    int sum = 0;

    for(int i = 0; strings[i] != NULL; i++)
        for(int j = 0; strings[i][j] != '\0'; j++)
            sum++;

    return sum;
}

/*
 * f("{"test","one","two"},'!') -> test!one!two
 */
char* merge_strings(char** strings,char delimiter){
    int size = getBytes(strings);
    char* output = malloc(sizeof(char)*size + 20); /* arbitrary 20 */
    for(int i = 0; strings[i] != NULL; i++){

        strcat(output,strings[i]);
        if(strings[i+1]) strcat(output,&delimiter);
    }

    return output;
}

int event_number(char** events){
    int i = 0;
    for(;events[i] != NULL; i++);

    return i;
}

char* long_to_string(long num,long size){
    char* output = malloc(sizeof(char) * NUM_DIGITS);
    snprintf(output,size,"%ld",num);

    return output;
}


/*
 * get_column("3:9:10",2) -> 9
 */
long get_column(char* event,long column){
    remove_newline(event);
    char** split = split_with_delimiter(event,':');
    long output = atol(split[column-1]);
    free(split);
    return output;
}


/*
 * append_value("1:2:3",10) -> "1:2:3:10"
 */
void append_value(char* event,char* const constval){

    char* constvalColon = malloc(sizeof(char) * PIPE_BUF);
    constvalColon[0] = '\0';
    strcat(constvalColon,":");
    strcat(constvalColon,constval);
    strcat(event,constvalColon);

}

/*
 * read a single line from input
 */
ssize_t readln(int fp, char* buf, long maxBytes){
    char c;
    int status;
    int nbytes = 0;

    while( (status = read(fp,&c,1)) >= 0 && nbytes < maxBytes){
        if(status == -1) return -1;

        buf[nbytes++] = c;
        if( (c == '\n' || c == '\0') && nbytes == 1) return 0;
        if(c == '\n') return nbytes;

    }
    return nbytes;
}
