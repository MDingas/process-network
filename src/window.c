#include "genLibrary.h" /* window() */

long window(char* operation,long* colvalues,long lines,long startIndex){

    long counter = 0;
    long min = LONG_MAX;
    long max = 0;
    long sum = 0;

    if(!strcmp(operation,"max")){
        if(startIndex == 0) return 0;
        for(int i = startIndex - 1; (counter < lines) && (i >= 0) ; i--){
            if(colvalues[i] > max)
                max = colvalues[i];
            counter++;
        }
        return max;
    }else if(!strcmp(operation,"min")){
        if(startIndex == 0) return 0;
        for(int i = startIndex - 1; (counter < lines) && (i >= 0) ; i--){
            if(colvalues[i] < min)
                min = colvalues[i];
            counter++;
        }
        return min;
    }else if(!strcmp(operation,"avg")){
        if(startIndex == 0) return 0;
        for(int i = startIndex - 1; (counter < lines) && (i >= 0) ; i--){
            sum += colvalues[i];
            counter++;
        }
        return(sum / (counter));

    }else if(!strcmp(operation,"sum")){
        if(startIndex == 0) return 0;
        for(int i = startIndex - 1; (counter < lines) && (i>= 0); i--){
            sum += colvalues[i];
            counter++;
        }
        return sum;

    }else{
        perror("[ERROR]operation unknown");
        exit(-1);
    }
}


/* Append to result the accumulated values of the avg/sum/min/max of n last lines *
 *
 * window 2 max 3
 * 1:2:3 -> 1:2:3:0
 * 4:5:6 -> 4:5:6:2
 * 7:8:9 -> 7:8:9:5
 * 0:0:0 -> 0:0:0:8
 */

int main(int argc, char** argv){

    if(argc != 4){
        perror("Usage: ./window <column> <avg/sum/min/max> <lines>");
        exit(-1);
    }

    long column = atol(argv[1]);
    char* operation = argv[2];
    long lines = atol(argv[3]);
    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);
    long colValues[PIPE_BUF];

    int i = 0;

    while(1){ /* perpetually open */

        while(readln(0,buffer,sizeof(buffer)) > 0){
            if(strlen(buffer) > 0){ /* no input and new line */
                remove_newline(buffer);
                if(event_number(split_with_delimiter(buffer,':')) < column){
                    perror("[ERROR]invalid column\n");
                }
                else{
                    long colVal = get_column(buffer,column);

                    colValues[i++] = colVal;
                    long out = window(operation,colValues,lines,i);
                    char* toAppend = long_to_string(out,NUM_DIGITS);
                    append_value(buffer,toAppend);
                    buffer[strlen(buffer)] = '\n';

                    if(write(1,buffer,strlen(buffer)) == -1)
                        error_writing_terminal();

                    clean_buffer(buffer,PIPE_BUF);
                }
            }
        }
    }

    return 0;
}
