#include "OperatorConsole.h"

#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem"
using namespace std;
void* userInputThread(void* arg) {
    OperatorConsole* console = static_cast<OperatorConsole*>(arg);
    std::string command;
    while (true) {
        std::cout << "Operator Console: Enter command: ";
        if (std::getline(std::cin, command)) {
            if (command == "exit") {
                std::cout << "Exiting user input thread..." << std::endl;
                break;
            }
            if (!command.empty()) {
                console->processUserCommand(command);
            }
        }
    }
    return NULL;
}
OperatorConsole::OperatorConsole() : server_coid(0) {

	this->logFile = creat( "/data/home/qnxuser/CommandLog.txt", S_IRUSR | S_IWUSR | S_IXUSR );

    if (pthread_create(&thread_id, NULL,userInputThread, this) != 0) {
        std::cerr << "Operator Console: Failed to start input thread\n";
    } else {
        isThreadActive = true;
    }
}

OperatorConsole::~OperatorConsole() {

    if (isThreadActive) {
        pthread_cancel(thread_id);
        pthread_join(thread_id, NULL);
    }
}

void OperatorConsole::log(const std::string& message) {
	int length = message.length();
	char buffer [length+1];
	strcpy(buffer, message.c_str());

	int size_written = write(logFile,buffer,length);
	/* test for error              */
	if( size_written != length ) {
		perror( "Error writing myfile.dat" );
	}

}

void OperatorConsole::processUserCommand(const std::string& command) {
    log(command);

    stringstream ss(command);
    string token;
    vector<string> tokens;
    char delimiter = ' ';
    compSysMsg dataReq;
    dataReq.ID = -1;
    dataReq.n = -1;
    dataReq.speedx = -1.0;
    dataReq.speedy = -1.0;
    dataReq.speedz = -1.0;

    while(getline(ss,token,delimiter)){
    	tokens.push_back(token);
    }

    if (tokens[0] == "requestInfo") {

    	dataReq.header.type = 0x03;
    	int id = stoi(tokens[1]);
    	dataReq.ID = id;

    	toComputerSys(dataReq);
    }
    else if(tokens[0]  == "changeSpeedx"){

		dataReq.header.type = 0x02;
		int id = stoi(tokens[1]);
		float speedx = stof(tokens[2]);
		dataReq.ID = id;
		dataReq.speedx = speedx;
		toComputerSys(dataReq);

    }
    else if(tokens[0]  == "changeSpeedy"){

		dataReq.header.type = 0x02;
		int id = stoi(tokens[1]);
		float speedy = stof(tokens[2]);
		dataReq.ID = id;
		dataReq.speedy = speedy;

		toComputerSys(dataReq);
    }
    else if(tokens[0]  == "changeSpeedz"){
    	;
		dataReq.header.type = 0x02;
		int id = stoi(tokens[1]);
		float speedz = stof(tokens[2]);
		dataReq.ID = id;
		dataReq.speedz = speedz;
		cout<<dataReq.ID<<dataReq.speedz<<endl;

		toComputerSys(dataReq);
    }
    else if(tokens[0]  == "changeViolFreq"){

		dataReq.header.type = 0x02;
		int n = stoi(tokens[1]);
		dataReq.n = n;
		toComputerSys(dataReq);
    }
    else{cout<<"Command is invalid!"<<endl;}
}
int OperatorConsole::toComputerSys(compSysMsg& data) {
    if ((server_coid = name_open(COMPUTER_SYSTEM_ATTACH_POINT, 0)) == -1) {
        std::cerr << "Operator Console: Failed to connect to server\n";
        return EXIT_FAILURE;
    }
    if (MsgSend(server_coid, &data, sizeof(data), NULL, 0) == -1) {
        std::cerr << "Operator Console: Failed to send message\n";
        return EXIT_FAILURE;
    }
    name_close(server_coid);
    return EXIT_SUCCESS;
}
