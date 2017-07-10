#include "genLibrary.h"

/* Connect write pipe of process A to read pipe of process B
 * BEFORE:
 * in -----> A ----> out
 * in -----> B ----> out
 * AFTER:
 *
 * in -----> A -----> B -----> out
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

	char buffer[PIPE_BUF]; cleanBuf(buffer,PIPE_BUF);
	int statusReadBuffer;
	int statusWriteBuffer;

	while( (statusReadBuffer = readln(statusR,buffer,sizeof(buffer))) > 0){
		if(statusReadBuffer < 0){ fprintf(stderr,"Error reading from first processes's output\n"); exit(-1); }

		statusWriteBuffer =	write(statusW,buffer,strlen(buffer));
		if(statusWriteBuffer < 0){ fprintf(stderr,"Error writing to second processes's input\n"); exit(-1); }
    cleanBuf(buffer,PIPE_BUF);
	}

	return 0;
}
