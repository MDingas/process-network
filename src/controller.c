#include "genLibrary.h" /* prefix_match(), readln(), clean_buffer(), PIPE_NAME_SIZE */
#include "controller.h" /* prototypes of all functions defined here */
#include "errors.h" /* simple error handling functions */

/* GLOBAL VARIABLES SETUP */

int g_nodes[MAX_IDS] = { 0 }; /* Index process ids by node ids , pid of node with id 1 gets saved on index 1 */
int g_savingScripts[MAX_IDS] = { 0 }; /* Saves by index the processes that save the output into a permanent file */
int g_connections[MAX_IDS][MAX_IDS] = { { 0 } }; /* Same idea for connections matrix, similar to connections matrix of graphs */
int g_mainPID = 0; /* mainPID if exists so only the main process handles SIGKILL and SIGINT signals */

/* HANDLER
 *
 * signal handler
 * currently handles SIGINT (CTR+C by user) and SIGQUIT (CTR+\) by user
 *
 */
void handler(int sig){
    char leave[10] = "n";

    fflush(stdin);

    /* User wants to terminate program */
    if(sig == SIGQUIT && g_mainPID == getpid() ){
        safe_printf("\nIt looks like you've tried to quit. Are you sure? [y/n]\n");
        if(read(0,leave,10) == -1)
            exit(-1);
        if(!strcmp(leave,"y\n")){
            clean();
            exit(0);
        }
        else{
            clean_buffer(leave,10);
            return;
        }

    }
    /* User wants to terminate his session */
    if(sig == SIGINT && g_mainPID == getpid() ){
        safe_printf("\nIt looks like you've tried to exit session. Are you sure? [y/n]\n");
        if(read(0,leave,sizeof(leave)) == -1)
            exit(-1);

        if(!strcmp(leave,"y\n")){
            safe_printf("[    ]Reseting...\n");
            clean();
            g_mainPID = 0;
            execl("bin/controller","bin/controller",NULL);
            clean();
            error_executing();
        }
        else{
            clean_buffer(leave,10);
            return;
        }


    }
}

/* CLEAN
 *	set every pid variable to zero
 *	remove every read/write named pipe
 *	delete every output file
 */
void clean(){
    char write[PIPE_NAME_SIZE];
    char read[PIPE_NAME_SIZE];

    safe_printf("Cleaning setup...\n");
    for(int i = 0; i < MAX_IDS; i++){
        if(g_nodes[i] != 0){
            sprintf(write, "%s%d%s", "temp/", i, "W");
            sprintf(read, "%s%d%s", "temp/", i, "R");
            kill(g_nodes[i],SIGKILL);
            unlink(write);
            unlink(read);
            g_nodes[i] = 0;
        }
    }
    for(int i = 0; i < MAX_IDS; i++){
        for(int j = 0; j < MAX_IDS; j++){
            if(g_connections[i][j] != 0){
                kill(g_connections[i][j],SIGKILL);
                g_connections[i][j] = 0;
            }
        }
        if(g_savingScripts[i] != 0){
            kill(g_savingScripts[i],SIGKILL);
            g_savingScripts[i] = 0;
        }
    }

    if (!fork()){ /* call script that deletes all output files in the output/ directory */
        execl("bin/deleteOutput","bin/deleteOutput",NULL);
        clean();
        error_executing();
    }
}

/* PRINT HELP
 *
 * showcase every possible command to user
 *
 */
void print_help(){
    safe_printf("\n\n");
    safe_printf("--->General Usage\n");
    safe_printf("\t\tOn startup, you must have a file in this directory\n");
    safe_printf("\t\tWhere each line corresponds to a node management command\n");
    safe_printf("--->Basic commands\n");
    safe_printf("\t\tconst <number>                             [Append a constant value to an event which will be read from stdin]\n");
    safe_printf("\t\tfilter <column> <condition> <number>       [Lets the event pass based on the passage of the filter]\n");
    safe_printf("\t\twindow <column> <operator> <lines>         [Outputs the operator of the n last lines (like the sum/avg/min of the last n lines on column c]\n");
    safe_printf("\t\tspawn <unix command>                       [Executes a unix command and appends to the event the command's exit status]\n");
    safe_printf("--->Node management commands\n");
    safe_printf("\t\tinject <id> <output command>\n             [Insert into a node some kind of output,for example: inject 1 cat input.txt]");
    safe_printf("\t\tnode <id> <basic command>                  [Iniciates a node with that id]\n");
    safe_printf("\t\tconnect <id> <listage of ids>              [Connects the node's in and output in a pipe\n");
    safe_printf("\t\tdisconnect <id>                            [Terminates a node]\n");
    safe_printf("--->Possible signals:\n");
    safe_printf("\t\tCTR+C to re-start session                  [Cleans every named pipe, removes every storage of process ids, complete reboot]\n");
    safe_printf("\t\tCTR+\\ to end program                       [Terminates process]\n");

    safe_printf("\n\n");
}

/* CREATE PIPE
 *
 * create two FIFOS, one read and one write, tagged with the node's id
 *
 */
int create_pipe(int id){
    char write[PIPE_NAME_SIZE];
    char read[PIPE_NAME_SIZE];

    sprintf(write, "%s%d%s", "temp/", id, "W");
    sprintf(read, "%s%d%s", "temp/", id, "R");

    if(mkfifo(write,0666) == -1){
        clean();
        error_mkfifo();
    }

    if(mkfifo(read,0666) == -1){
        clean();
        error_mkfifo();
    }

    return 0;
}

/* CONNECT NODES
 *
 * Call a process that reads from a node's write pipe and writes to a node's read pipe
 *
 *
 */
int connect_nodes(char* command){
    safe_printf("[START]Connecting nodes...\n");
    char** splitCommands = split_with_delimiter(command,' ');

    if(event_number(splitCommands) < 3){
        perror("[ERROR]usage: connect <id1> <list of ids>\n");
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
    sprintf(write,"temp/%dW",id);

    /* connect node write to the N read nodes */
    for(int i = 0; connectIds[i] != -1; i++){
        int pid;

        if( (pid = fork()) == -1){
            clean();
            error_forking();
        }

        /* Dad process saves the pid as the connection between the 2 processes */
        if(pid > 0){ g_connections[id][connectIds[i]] = pid; }

        /* Son process will call process that redirects pipes */
        if(pid == 0){
            char read[PIPE_NAME_SIZE];
            sprintf(read,"temp/%dR",connectIds[i]);
            execl("bin/link","bin/link",write,read,NULL);
            clean();
            error_executing();
        }

    }

    safe_printf("[SUCC]Nodes connected\n");
    free(splitCommands);
    return 0;
}

/* DISCONNECT NODES
 *  First check if there's a pid in that connection (you can only kill what exists )
 *  If it does exist, kill that process (which is the bridge between the processes, thus the connection between them )
 */
int disconnect_nodes(char* command){
    safe_printf("[START]disconnecting nodes...\n");
    char** splitCommands = split_with_delimiter(command,' ');

    if(event_number(splitCommands) != 3){
        perror("[ERROR]usage: disconnect <id>\n");
        exit(-1);
    }


    int id1 = atoi(splitCommands[1]);
    int id2 = atoi(splitCommands[2]);

    if(g_connections[id1][id2] != 0){
        int process = g_connections[id1][id2];
        int status = kill(process,SIGKILL);
        if(status == -1){
            clean();
            perror("[ERROR]killing proccess\n");
            exit(-1);
        }
        /* reset */
        g_connections[id1][id2] = 0;
    }


    safe_printf("[SUCC]nodes disconnected\n");
    free(splitCommands);

    return 0;
}


/* CREATE NODE:
   Parse command: Is it one of the 4 functions that are pre defined or is it a unix command?
   Insert pid into array of pids
   Create a pair of read/write named pipes
   Execute command
   */
int create_node(char* command){
    safe_printf("[START]Creating node...\n");
    char** splitCommand = split_with_delimiter(command,' ');

    if(event_number(splitCommand) < 3){
        clean();
        perror("[ERROR]usage: node <id> <commands with args>\n");
        exit(-1);
    }

    int commandFlag = -1;

    /* Split command into manageable chunks to parse */
    char** args = malloc(sizeof(char*) * PIPE_BUF);
    for(int i = 0; i < PIPE_BUF; i++)
        args[i] = malloc(sizeof(char) * PIPE_BUF);

    char fullPath[100];
    sprintf(fullPath,"bin/%s",splitCommand[2]);
    splitCommand[2] = fullPath; /* const -> bin/const */

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
        error_forking();
    }


    /* son sets up fifos and runs command */
    if(pd == 0){
        if(create_pipe(id) == -1){ /* Creates two pipes: in and out, named with id */
            clean();
            error_pipe();
        }


        /* Position write and read pipes accordingly */
        char write[PIPE_NAME_SIZE];
        char read[PIPE_NAME_SIZE];

        char file[PIPE_NAME_SIZE];
        sprintf(file,"output/%dout",id);


        sprintf(write,"temp/%dW",id);
        sprintf(read,"temp/%dR",id);

        /* Open fifos */
        int openW = open(write,O_WRONLY);
        int openR = open(read,O_RDONLY);

        /* Redirect input and output*/
        dup2(openR,0); /* stop reading from stdin, read from openR */
        close(openR);
        dup2(openW,1); /* stop writting to stdout, write to openW */
        close(openW);

        /* Execute one of the four made commands */
        if(commandFlag != -1){
            execv(splitCommand[2],args);
        }
        /* Execute unix command */
        else{
            execvp(splitCommand[2],args);

        }

        clean();
        error_executing();
    }

    if(pd > 0){
        g_nodes[id] = pd;
    }

    /* dad process saved pid */

    safe_printf("[SUCC]Node created\n");
    free(splitCommand);
    return 0;
}

/* MUTE ALL LEECHERS EXCEPT
 *
 * Pause all connections between nodes, except one specified
 * This essentially means create a single path for input to flow
 *
 */
void mute_all_leechers_except(int seeder,int exception){
    for(int i = 0; i < MAX_IDS; i++){
        if (i != exception && g_connections[seeder][i] != 0) {
            kill(g_connections[seeder][i],SIGTSTP);
        }
    }
}

/* OPEN ALL LEECHERS
 *
 * Reset all the connections, by continuing every process
 *
 */
void open_all_leechers(int seeder){
    /* SIGCONT sent to processes that are not stoped simply ignore the signal */
    for(int i = 0; i < MAX_IDS; i++){
        if (g_connections[seeder][i] != 0 )
            kill(g_connections[seeder][i],SIGCONT);
    }
}

/* FIND ONLY SON
 *
 * Traverse connections matrix and find the only non null value
 *
 */
int find_only_son(int seeder){
    for (int i = 0; i < MAX_IDS; i++)
        if (g_connections[seeder][i] != 0)
            return i;
    return -1;
}

/* EXISTS BIFURCATION
 *
 * Boolean return if the connection matrix has bifurcations
 *
 */

int exists_bifurcation(){
    int answer = 0;

    for (int i = 0; i < MAX_IDS; i++) {
        int seeders = 0;
        for (int j = 0; j < MAX_IDS; j++) {
            if (g_connections[i][j] != 0) {
                seeders++;
            }
        }
        if (seeders >= 2) {
            answer = 1;
        }
    }

    return answer;
}

/* LEECHERS
 *
 * Count how many leechers a node has (count non null connections in connection matrix)
 *
 */
int leechers(int seeder){
    int leechers_num = 0;
    for(int i = 0; i < MAX_IDS; i++)
        if(g_connections[seeder][i] != 0)
            leechers_num++;
    return leechers_num;
}

int total_nodes(){
    int total = 0;
    for(int i = 0; i < MAX_IDS; i++)
        if(g_nodes[i] != 0)
            total++;
    return total;
}

/* HANDLE INJECTION
 *
 * Before calling injection, check for forking of nodes, that require injecting multiple times
 * No child nodes, inject
 * One child node, recursive call to child
 * Multiple child nodes, isolate path to one child and recursive call
 *
 * This assures every possible path in the node graph gets a continuous piece of data
 *
 */
void handle_injection(char* command,int current_node){

    if (leechers(current_node) == 0) { /* we reached a node with no sons, inject */

        char current_node_as_string[PIPE_NAME_SIZE];
        sprintf(current_node_as_string,"%d",current_node);
        inject_node(command);

        int pid;
        if (g_savingScripts[current_node] == 0) { /* only call saving script if there isnt one there already */

            if ( (pid = fork()) == 0) {
                execl("bin/saveResultsScript","bin/saveResultsScript",current_node_as_string,NULL);
                clean();
                error_executing();
            }

            g_savingScripts[current_node] = pid;
        }
        if (exists_bifurcation()) { /* only sleep if there are cases of bifurcation */
            sleep(total_nodes() / 2 ); /* sloppy estimation, change later */
        }


    } else if (leechers(current_node) == 1) { /* only one son, continue chain without pausing communication */

        int only_son = find_only_son(current_node);
        if(only_son != -1)
            handle_injection(command,only_son);
    } else{ /* multiple sons, do one communication at a time */

        for(int i = 0; i < MAX_IDS; i++){
            if (g_connections[current_node][i] != 0){
                mute_all_leechers_except(current_node,i);
                handle_injection(command,i);
                open_all_leechers(current_node);
            }
        }
    }
}

/* INJECT NODE
 * Execute the command in the argument, and redirect the output to the node's read pipe
 */
int inject_node(char* command){
    safe_printf("[START]injecting node\n");

    remove_newline(command);
    char** splitCommand = split_with_delimiter(command,' ');
    if(event_number(splitCommand) < 3){
        clean();
        perror("[ERROR]usage: inject <id> <commands w/ args>\n");
        exit(-1);
    }
    char** cmds = malloc(event_number(splitCommand) - 2);
    for(int i = 0; i < event_number(splitCommand) - 2; i++)
        cmds[i] = malloc(sizeof(char) * PIPE_BUF);

    int i = 2, j = 0;
    for(; splitCommand[i] != NULL; i++, j++){
        cmds[j] = splitCommand[i];
    }
    cmds[j] = NULL;
    int id = atoi(splitCommand[1]);

    if ( !fork() ) {

        /* Create pipe */

        char bad[PIPE_NAME_SIZE];
        sprintf(bad,"temp/%dR",id);
        int badop = open(bad,O_WRONLY);
        dup2(badop,1);
        close(badop);


        if (cmds[0][0] != '.') {
            execvp(cmds[0],cmds);
        } else{
            execv(cmds[0],cmds);
        }

        clean();
        error_executing();

    }

    safe_printf("[SUCC]node injected\n");
    free(splitCommand);
    return 0;
}

/* PARSE COMMAND
 *
 * Filter what command was typed and call the correct function
 *
 */
int parse_command(char* command){

    if(prefix_match("node",command)){
        if(create_node(command) == -1){
            clean();
            perror("[ERROR]creating node\n");
            exit(-1);
        }

    }else if(prefix_match("inject",command)){
        /* parsing */
        remove_newline(command);
        char** splitCommand = split_with_delimiter(command,' ');
        if(event_number(splitCommand) < 3){
            clean();
            perror("[ERROR]usage: inject <id> <commands w/ args>\n");
            exit(-1);
        }
        int id = atoi(splitCommand[1]);

        handle_injection(command,id);
        free(splitCommand);

    }else if(prefix_match("disconnect",command)){
        if(disconnect_nodes(command) == -1){
            clean();
            perror("[ERROR]disconnecting nodes\n");
            exit(-1);
        }

    }else if(prefix_match("connect",command)){
        if(connect_nodes(command) == -1){
            clean();
            perror("[ERROR]connecting nodes\n");
            exit(-1);
        }
    }else if(prefix_match("help",command)){
        print_help();
    }

    return 0;
}

/* MAIN PARSER
 *
 * Start program, read config file, and permanently read terminal input
 *
 */
void main_parser(){

    safe_printf("What file do you want to parse:(help if this doesn't make sense)");


    char file[PIPE_BUF];

    if( (readln(0,file,sizeof(file))) == -1){
        clean();
        error_reading_terminal();
    }


    if(!strcmp(file,"help\n")){
        print_help();
        execl("bin/controller","bin/controller",NULL);
        clean();
        error_executing();
    }

    remove_newline(file);


    /* File setup */

    int fp;

    if( (fp = open(file,O_RDONLY)) == -1){
        clean();
        error_opening_file();
    }

    char buffer[PIPE_BUF]; clean_buffer(buffer,PIPE_BUF);

    while(readln(fp,buffer,PIPE_BUF) > 0){
        remove_newline(buffer);
        if (write(1,"[    ]parsing ",strlen("[    ]parsing ")) == -1){
            clean();
            error_writing_terminal();
        }
        if (write(1,buffer,strlen(buffer)) == -1){
            clean();
            error_writing_terminal();
        }
        if (write(1,"\n",1) == -1){
            clean();
            error_writing_terminal();
        }

        parse_command(buffer);
        clean_buffer(buffer,PIPE_BUF);
    }

    while(1){
        safe_printf("User input mode\n");

        if(readln(0,buffer,sizeof(buffer)) == -1){
            clean();
            error_reading_terminal();
        }

        parse_command(buffer);
    }


    close(fp);
}


/* MAIN
 *
 * Setup signals, store mainPID, call main parser
 *
 */
int main(){
    safe_printf("Main start\n");

    /* Signal handling */
    /* Attributing SIGINT is a little bit more fancy because
     *  I want the signal to keep being received even by a
     *  child process */
    struct sigaction sigint_sa;
    sigint_sa.sa_handler = handler;
    sigemptyset(&sigint_sa.sa_mask);
    sigint_sa.sa_flags = SA_NODEFER; /* Dont block signals to any child process */
    if (sigaction(SIGINT, &sigint_sa, NULL) == -1){
        clean();
        perror("[ERROR]atributing signalhandler\n");
        exit(-1);
    }

    signal(SIGQUIT,handler);

    /* Main pid setup */
    if(g_mainPID == 0) g_mainPID = getpid();

    main_parser();

    safe_printf("Main exit\n");
}
