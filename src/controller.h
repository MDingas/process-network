
void handler(int sig);
void mute_all_leechers_except(int seeder,int exception);
void open_all_leechers(int seeder);
int leechers(int seeder);
void clean();
void printHelp();
int createPipe(int id);
int connectNodes(char* command);
int disconnectNodes(char* command);
int createNode(char* command);
int find_only_son(int seeder);
void connectInjections(int id1,int id2);
int injectNode(char* command);
int parseCommand(char* command);
void mainParser();
