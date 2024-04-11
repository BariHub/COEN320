#include "OperatorConsole.h"

#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem"
using namespace std;
//start routine for operator console thread
void* userInputThread(void* arg) {
    OperatorConsole& console = *(OperatorConsole*)(arg);
    std::string command;
    while (true) {
        //obtain commands from user input and send them to the process user command function
        if (std::getline(std::cin, command)) {
            if (command == "exit") {
                std::cout << "Exiting user input thread..." << std::endl;
                break;
            }
            if (!command.empty()) {
                console.processUserCommand(command);
            }
        }
    }
    return NULL;
}
//constructor
OperatorConsole::OperatorConsole() : server_coid(0) {

	this->logFile = creat( "/data/home/qnxuser/CommandLog.txt", S_IRUSR | S_IWUSR | S_IXUSR );
	//create operator console thread
    if (pthread_create(&thread_id, NULL,userInputThread, (void *)this) != 0) {
        std::cerr << "Operator Console: Failed to start input thread\n";
    }
}
//destructor
OperatorConsole::~OperatorConsole() {
}
//logs commands into the log file
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
    log(command); // log commands in file

    // break the command to command , id(if applicable) , speed(if applicable) , number of seconds( if applicable)
    stringstream ss(command);
    string token;
    vector<string> tokens;
    char delimiter = ' ';
    compSysMsg dataReq;
    //initialize data to -1 to avoid overwriting data
    dataReq.ID = -1;
    dataReq.n = -1;
    dataReq.speedx = -1.0;
    dataReq.speedy = -1.0;
    dataReq.speedz = -1.0;

    while(getline(ss,token,delimiter)){
    	tokens.push_back(token);
    }
    //iterate to find which command is to be dealt with
    if (tokens[0] == "requestInfo") {

    	dataReq.header.type = 0x03; //indicates req info to computer system and display system
    	int id = stoi(tokens[1]);
    	dataReq.ID = id;

    	toComputerSys(dataReq);// send request to computer system
    }
    else if(tokens[0]  == "changeSpeedx"){

		dataReq.header.type = 0x02;//indicates change of speed command to the computer system
		int id = stoi(tokens[1]);
		float speedx = stof(tokens[2]);
		dataReq.ID = id;
		dataReq.speedx = speedx;
		dataReq.speedy = -1.0;
		dataReq.speedz = -1.0;
		toComputerSys(dataReq);

    }
    else if(tokens[0]  == "changeSpeedy"){

		dataReq.header.type = 0x02;//indicates change of speed command to the computer system
		int id = stoi(tokens[1]);
		float speedy = stof(tokens[2]);
		dataReq.ID = id;
		dataReq.speedx = -1.0;
		dataReq.speedy = speedy;
		dataReq.speedy = -1.0;
		toComputerSys(dataReq);
    }
    else if(tokens[0]  == "changeSpeedz"){
		dataReq.header.type = 0x02;//indicates change of speed command to the computer system
		int id = stoi(tokens[1]);
		float speedz = stof(tokens[2]);
		dataReq.ID = id;
		dataReq.speedx = -1.0;
		dataReq.speedy = -1.0;
		dataReq.speedz = speedz;
		toComputerSys(dataReq);
    }
    else if(tokens[0]  == "changeViolFreq"){

		dataReq.header.type = 0x02;//indicates change of number of seconds to compute the violation interval command to the computer system
		int n = stoi(tokens[1]);
		dataReq.n = n;
		toComputerSys(dataReq);
    }
    else{cout<<"Command is invalid!"<<endl;}
}
//function used to send command to the computer system using the computer system attach point defined at the start
int OperatorConsole::toComputerSys(compSysMsg& data) {
    if ((server_coid = name_open(COMPUTER_SYSTEM_ATTACH_POINT, 0)) == -1) {
        std::cerr << "Operator Console: Failed to connect to server\n";
        return EXIT_FAILURE;
    }
    if (MsgSend(server_coid, &data, sizeof(data), NULL, 0) == -1) {//sends the message to the computer system
        std::cerr << "Operator Console: Failed to send message\n";
        return EXIT_FAILURE;
    }
    name_close(server_coid);
    return EXIT_SUCCESS;
}
