#include "genLibrary.h" /* prefixMatch(), readln(), cleanBuf(), PIPE_NAME_SIZE */
#include "controller.h" /* prototypes of all functions defined here */
#include "errors.h" /* simple error handling functions */

/* GLOBAL VARIABLES SETUP */

int nodes[MAX_IDS] = { 0 }; /* Index process ids by node ids , pid of node with id 1 gets saved on index 1 */
int connections[MAX_IDS][MAX_IDS] = { { 0 } }; /* Same idea for connections matrix, similar to connections matrix of graphs */
int mainPID = 0; /* mainPID if exists so only the main process handles SIGKILL and SIGINT signals */

void handler(int sig){
    char leave[10] = "n";

    fflush(stdin);

    /* User wants to terminate program */
    if(sig == SIGQUIT && mainPID == getpid() ){
        safePrintf("\nIt looks like you've tried to quit. Are you sure? [y/n]\n");
        if(read(0,leave,10) == -1)
            exit(-1);
        if(!strcmp(leave,"y\n")){
            clean();
            exit(0);
        }
        else{
            cleanBuf(leave,10);
            return;
        }

    }
    /* User wants to terminate his session */
    if(sig == SIGINT && mainPID == getpid() ){
        safePrintf("\nIt looks like you've tried to exit session. Are you sure? [y/n]\n");
        if(read(0,leave,sizeof(leave)) == -1)
            exit(-1);

        if(!strcmp(leave,"y\n")){
            safePrintf("[    ]Reseting...\n");
            clean();
            mainPID = 0;
            execl("./controller","./controller",NULL);
            clean();
            errorExecuting();
        }
        else{
            cleanBuf(leave,10);
            return;
        }


    }
}

/* CLEAN
 *	set every pid variable to zero
 *	remove every read/write named pipe
 */
void clean(){
    char write[PIPE_NAME_SIZE];
    char read[PIPE_NAME_SIZE];

    safePrintf("Cleaning setup...\n");
    for(int i = 0; i < MAX_IDS; i++){
        if(nodes[i] != 0){
            sprintf(write, "%s%d%s", "./temp/", i, "W");
            sprintf(read, "%s%d%s", "./temp/", i, "R");
            kill(nodes[i],SIGKILL);
            unlink(write);
            unlink(read);
            nodes[i] = 0;
        }
    }
    for(int i = 0; i < MAX_IDS; i++)
        for(int j = 0; j < MAX_IDS; j++){
            if(connections[i][j] != 0){
                kill(connections[i][j],SIGKILL);
                connections[i][j] = 0;
            }
        }
}

void printHelp(){
    safePrintf("\n\n");
    safePrintf("--->General Usage\n");
    safePrintf("\t\tOn startup, you must have a file in this directory\n");
    safePrintf("\t\tWhere each line corresponds to a node management command\n");
    safePrintf("--->Basic commands\n");
    safePrintf("\t\tconst <number>                             [Append a constant value to an event which will be read from stdin]\n");
    safePrintf("\t\tfilter <column> <condition> <number>       [Lets the event pass based on the passage of the filter]\n");
    safePrintf("\t\twindow <column> <operator> <lines>         [Outputs the operator of the n last lines (like the sum/avg/min of the last n lines on column c]\n");
    safePrintf("\t\tspawn <unix command>                       [Executes a unix command and appends to the event the command's exit status]\n");
    safePrintf("--->Node management commands\n");
    safePrintf("\t\tnode <id> <basic command>                  [Iniciates a node with that id]\n");
    safePrintf("\t\tconnect <id> <listage of ids>              [Connects the node's in and output in a pipe\n");
    safePrintf("\t\tdisconnect <id>                            [Terminates a node]\n");
    safePrintf("--->Possible signals:\n");
    safePrintf("\t\tCTR+C to re-start session                  [Cleans every named pipe, removes every storage of process ids, complete reboot]\n");
    safePrintf("\t\tCTR+\\ to end program                       [Terminates process]\n");

    safePrintf("\n\n");
}

int createPipe(int id){
    char write[PIPE_NAME_SIZE];
    char read[PIPE_NAME_SIZE];

    sprintf(write, "%s%d%s", "./temp/", id, "W");
    sprintf(read, "%s%d%s", "./temp/", id, "R");

    if(mkfifo(write,0666) == -1){
        clean();
        errorMkfifo();
    }

    if(mkfifo(read,0666) == -1){
        clean();
        errorMkfifo();
    }

    return 0;
}

int connectNodes(char* command){
    safePrintf("[START]Connecting nodes...\n");
    char** splitCommands = splitAt(command,' ');

    if(eventNums(splitCommands) < 3){
        fprintf(stderr,"[ERROR]usage: connect <id1> <list of ids>\n");
        exit(-1);
    }

    int id = atoi(splitCommands[1]); /* Main id to connect to */
    int connectIds[MAX_IDS]; /* List of ids that are going to be connected */

    /* Fill connectIds array */
    int i = 2; /* where the "connect to" ids begin in splitCommands */
    int j = 0;
    for(; splitCommands[i] != NULL; i++, j++)
        connectIds[j] = atoi(splitCommands[i]);
    connectIds[j] = -1;

    char write[PIPE_NAME_SIZE];
    sprintf(write,"./temp/%dW",id);

    /* connect node write to the N read nodes */
    for(int i = 0; connectIds[i] != -1; i++){
        int pid;
        if( (pid = fork()) == -1){
            clean();
            errorForking();
        }

        /* Dad process saves the pid as the connection between the 2 processes */
        if(pid > 0){ connections[id][connectIds[i]] = pid; }
        /* Son process will call process that redirects pipes */
        if(pid == 0){
            char read[PIPE_NAME_SIZE];
            sprintf(read,"./temp/%dR",connectIds[i]);
            execl("./link","./link",write,read,NULL);
            clean();
            errorExecuting();
        }

    }

    safePrintf("[SUCC]Nodes connected\n");
    return 0;
}

/* DISCONNECT NODES
 *  First check if there's a pid in that connection (you can only kill what exists )
 *  If it does exist, kill that process (which is the bridge between the processes, thus the connection between them )
 */
int disconnectNodes(char* command){
    safePrintf("[START]disconnecting nodes...\n");
    char** splitCommands = splitAt(command,' ');

    if(eventNums(splitCommands) != 3){
        fprintf(stderr,"[ERROR]usage: disconnect <id>\n");
        exit(-1);
    }


    int id1 = atoi(splitCommands[1]);
    int id2 = atoi(splitCommands[2]);

    if(connections[id1][id2] != 0){
        int process = connections[id1][id2];
        int status = kill(process,SIGKILL);
        if(status == -1){
            clean();
            fprintf(stderr,"[ERROR]killing proccess\n");
            exit(-1);
        }
        /* reset */
        connections[id1][id2] = 0;
    }


    safePrintf("[SUCC]nodes disconnected\n");

    return 0;
}


/* CREATE NODE:
   Parse command: Is it one of the 4 functions that are pre defined or is it a unix command?
   Insert pid into array of pids
   Create a pair of read/write named pipes
   Execute command
   */
int createNode(char* command){
    safePrintf("[START]Creating node...\n");
    char** splitCommand = splitAt(command,' ');

    if(eventNums(splitCommand) < 3){
        fprintf(stderr,"[ERROR]usage: node <id> <commands with args>\n");
        exit(-1);
    }

    int commandFlag = -1;

    /* Split command into manageable chunks to parse */
    char** args = malloc(sizeof(char*) * PIPE_BUF);
    for(int i = 0; i < PIPE_BUF; i++)
        args[i] = malloc(sizeof(char) * PIPE_BUF);

    /* tag kind of command running */
    if(!strcmp(splitCommand[2],"const")){
        commandFlag = 1;
    }else if(!strcmp(splitCommand[2],"filter")){
        commandFlag = 2;
    }else if(!strcmp(splitCommand[2],"window")){
        commandFlag = 3;
    }else if(!strcmp(splitCommand[2],"spawn")){
        commandFlag = 4;
    }

    switch(commandFlag){
        case(1):
            splitCommand[2] = "./const";
            break;
        case(2):
            splitCommand[2] = "./filter";
            break;
        case(3):
            splitCommand[2] = "./window";
            break;
        case(4):
            splitCommand[2] = "./spawn";
            break;
        default:
            break;
    }

    /* Save id and command seperately */
    int id = atoi(splitCommand[1]);
    int i = 2; /* where actual command starts */
    int j = 0;
    for(; splitCommand[i] != NULL; i++,j++)
        args[j] = splitCommand[i];
    args[j] = NULL;


    /* Execute command with child process */

    pid_t pd;

    if( (pd = fork()) == -1){
        clean();
        errorForking();
    }

    if(pd > 0){
        nodes[id] = pd;
    }

    /* son sets up fifos and runs command */
    if(pd == 0){
        if(createPipe(id) == -1){ /* Creates two pipes: in and out, named with id */
            clean();
            errorPipe();
        }


        /* Position write and read pipes accordingly */
        char write[PIPE_NAME_SIZE];
        char read[PIPE_NAME_SIZE];
        sprintf(write,"./temp/%dW",id);
        sprintf(read,"./temp/%dR",id);

        /* Open fifos */
        int openW = open(write,O_WRONLY);
        int openR = open(read,O_RDONLY);

        /* Redirect input and output*/
        dup2(openR,0); /* stop reading from stdin, read from openR */
        dup2(openW,1); /* stop writting to stdout, write to openW */

        /* Execute one of the four made commands */
        if(commandFlag != -1){
            execv(splitCommand[2],args);
        }
        /* Execute unix command */
        else{
            execvp(splitCommand[2],args);

        }
        errorExecuting();
    }


    /* dad process saved pid */

    safePrintf("[SUCC]Node created\n");
    return 0;
}

/* INJECT NODE
 * create inject pipe to bridge processes
 */
int injectNode(char* command){
    safePrintf("[START]injecting node\n");

    removeNewline(command);
    char** splitCommand = splitAt(command,' ');
    if(eventNums(splitCommand) < 3){
        clean();
        fprintf(stderr,"[ERROR]usage: inject <id> <commands w/ args>\n");
        exit(-1);
    }
    char** cmds = malloc(eventNums(splitCommand) - 2);
    for(int i = 0; i < eventNums(splitCommand) - 2; i++)
        cmds[i] = malloc(sizeof(char) * PIPE_BUF);

    int i = 2, j = 0;
    for(; splitCommand[i] != NULL; i++, j++){
        cmds[j] = splitCommand[i];
    }
    cmds[j] = NULL;
    int id = atoi(splitCommand[1]);

    pid_t pid = fork();

    if(pid == 0){


        /* Create pipe */

        char bad[PIPE_NAME_SIZE];
        sprintf(bad,"./temp/%dR",id);
        int badop = open(bad,O_WRONLY);
        dup2(badop,1);


        if(cmds[0][0] != '.')
            execvp(cmds[0],cmds);
        else
            execv(cmds[0],cmds);


        clean();
        errorExecuting();

    }

    safePrintf("[SUCC]node injected\n");
    return 0;
}

int parseCommand(char* command){

    if(prefixMatch("node",command)){
        if(createNode(command) == -1){
            clean();
            fprintf(stderr,"[ERROR]creating node\n");
            exit(-1);
        }

    }else if(prefixMatch("inject",command)){
        if(injectNode(command) == -1){
            clean();
            fprintf(stderr,"[ERROR]injecting node\n");
            exit(-1);
        }

    }else if(prefixMatch("disconnect",command)){
        if(disconnectNodes(command) == -1){
            clean();
            fprintf(stderr,"[ERROR]disconnecting nodes\n");
            exit(-1);
        }

    }else if(prefixMatch("connect",command)){
        if(connectNodes(command) == -1){
            clean();
            fprintf(stderr,"[ERROR]connecting nodes\n");
            exit(-1);
        }
    }else if(prefixMatch("help",command)){
        printHelp();
    }

    return 0;
}

void mainParser(){

    safePrintf("What file do you want to parse:(help if this doesn't make sense)");


    char file[PIPE_BUF];

    if( (readln(0,file,sizeof(file))) == -1){
        clean();
        errorReadingTerminal();
    }


    if(!strcmp(file,"help\n")){
        printHelp();
        execl("./controller","./controller",NULL);
        clean();
        errorExecuting();
    }

    removeNewline(file);


    /* File setup */

    int fp;

    if( (fp = open(file,O_RDONLY)) == -1){
        clean();
        errorOpeningFile();
    }

    char buffer[PIPE_BUF]; cleanBuf(buffer,PIPE_BUF);

    while(readln(fp,buffer,PIPE_BUF) > 0){
        removeNewline(buffer);
        write(1,"[    ]parsing ",strlen("[    ]parsing "));
        write(1,buffer,strlen(buffer));
        write(1,"\n",1);

        parseCommand(buffer);
        cleanBuf(buffer,PIPE_BUF);
    }

    while(1){
        safePrintf("User input mode\n");

        if(readln(0,buffer,sizeof(buffer)) == -1){
            clean();
            errorReadingTerminal();
        }

        parseCommand(buffer);
    }


    close(fp);
}


int main(){
    safePrintf("Main start\n");

    /* Signal handling */
    /* Attributing SIGINT is a little bit more fancy because
     *  I want the signal to keep being received even by a
     *  child process */
    struct sigaction sigint_sa;
    sigint_sa.sa_handler = handler;
    sigemptyset(&sigint_sa.sa_mask);
    sigint_sa.sa_flags = SA_NODEFER; /* Dont block signals to any child process */
    if (sigaction(SIGINT, &sigint_sa, NULL) == -1){
        fprintf(stderr,"[ERROR]atributing signalhandler\n");
        exit(-1);
    }

    signal(SIGQUIT,handler);

    /* Main pid setup */
    if(mainPID == 0) mainPID = getpid();

    mainParser();

    safePrintf("Main exit\n");
}
