#include "OperatorConsole.h"
#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem"

// check if commands match then return speed/FL/pos value
actionCommand stringToActionCommand (const std::string& inString) {
    if (inString == "change speed") return adjustSpeed;
    if (inString == "change altitude") return adjustFL;
    if (inString == "change position") return adjustPosition;
    else return invalidCommand;
}


int OperatorConsole::toComputerSys(compsys_msg data) {
	if ((server_coid = name_open(COMPUTER_SYSTEM_ATTACH_POINT, 0)) == -1) {
			std::cout << "Operator Console: Failed connection to server \n\n";
			return EXIT_FAILURE;
	}
	if (MsgSend(server_coid, &data, sizeof(data), 0, 0) == -1) {
		std::cout << "Operator Console: Failed to send message \n\n";
		return EXIT_FAILURE;
	}
	name_close(server_coid);
	return EXIT_SUCCESS;
}

// apply command to plane data
void OperatorConsole::applyCommandToPlane(compsys_msg& data, const std::string& command, int amount) {
    actionCommand cmd = stringToActionCommand(command);
    switch (cmd) {
        case adjustSpeed:
            data.arrivalVelX = amount;
            break;
        case adjustFL:
            data.arrivalVelY = amount;
            break;
        case adjustPosition:
            data.arrivalVelZ = amount;
            break;
        case invalidCommand:
            std::cout << "Operator Console: Invalid command. Please try again" << std::endl;
            break;
    }
}


void OperatorConsole::processCommandsForPlane() {
	vector<string> commandTypes = {"change speed", "change altitude", "change position", "change speed", "change altitude"};
	vector<int> commandValues = {100, 200, 300, 400, 500};
	vector<int> targetPlaneIDs = {6, 6, 2, 6, 6};


    for (size_t i = 0; i < commandTypes.size(); i++) {
        for (const auto& plane : atc.planes) {
            if (targetPlaneIDs[i] == plane->ID) {
                compsys_msg data;
                data.hdr.type = 0x02;
                data.ID = targetPlaneIDs[i];

                data.arrivalPosX = plane->arrivalPosX;
                data.arrivalPosY = plane->arrivalPosY;
                data.arrivalPosZ = plane->arrivalPosZ;
                data.arrivalVelX = plane->arrivalVelX;
                data.arrivalVelY = plane->arrivalVelY;
                data.arrivalVelZ = plane->arrivalVelZ;

                applyCommandToPlane(data, commandTypes[i], commandValues[i]);
                toComputerSys(data);
                break;
            }
        }
    }

}

void OperatorConsole::getCommands() {

    cTimer timer(5,0,25,0);

    while (true) {
        timer.wait_next_activation();

        if (atc.planes.empty()) {
            std::cout << "Operator Console: Airspace empty, no airplanes to change";
        } else {
            processCommandsForPlane();
        }
    }
}

void* operator_console_start_routine(void *arg)
{
	OperatorConsole& operatorConsole = *(OperatorConsole*) arg;
	operatorConsole.getCommands();
	return NULL;
}

OperatorConsole::OperatorConsole(ATC atc) {
		this->atc=atc;
		this->server_coid = 0;
		if(pthread_create(&thread_id,NULL,operator_console_start_routine,(void *) this)!=EOK)
		{
			std::cout << "Operator Console: Failed to start\n\n";
		}
}

OperatorConsole::~OperatorConsole() {
}
