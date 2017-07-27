#include "genLibrary.h"

/* Connect write pipe of process A to read pipe of process B
 * BEFORE:
 * in -----> A ----> out
 * in -----> B ----> out
 * AFTER:
 *
 * in -----> A -----> B -----> out
 *
 * Essentially this program reads from A's write pipe and writes to B's read pipe, perpetually
 */

int main(int argc,char** argv){
    if(argc != 3){
        fprintf(stderr,"Usage: ./link <processId1> <processId2>\n");
        exit(-1);
    }

    int statusR = open(argv[1],O_RDONLY);
    int statusW = open(argv[2],O_WRONLY);

    if(statusR == -1){ fprintf(stderr,"[ERROR]opening read pipe %s, write pipe was %s\n",argv[1],argv[2]); exit(-1); }
    if(statusW == -1){ fprintf(stderr,"[ERROR]opening write pipe %s\n",argv[2]); exit(-1); }

    /* read from A while possible, and write to B */

    dup2(statusR,0); close(statusR);
    dup2(statusW,1); close(statusW);

    execlp("cat","cat",NULL);

    return 0;
}
