
void handler(int sig);
void clean();
void printHelp();
int createPipe(int id);
int connectNodes(char* command);
int disconnectNodes(char* command);
int createNode(char* command);
void connectInjections(int id1,int id2);
int injectNode(char* command);
int parseCommand(char* command);
void mainParser();
