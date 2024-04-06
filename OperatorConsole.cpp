#include "OperatorConsole.h"
#include "Radar.h"
#include <iostream>
#include <cmath>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <atomic>
#include <fstream>
#include <thread>

using namespace std;

enum CommandType {
    INFO_COMMAND = 1,
    CHANGE_LEVEL = 2,
    CHANGE_SPEED = 3,
    CHANGE_POSITION = 4,
};

OperatorConsole::OperatorConsole() {
    // open connection with server using name_open
	// connect to channel attach_point
    if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {
        perror("Error occurred while attaching to the channel");
        exit(EXIT_FAILURE);
    }
}

OperatorConsole::~OperatorConsole() {
	// signal the thread to terminate
    shouldTerminate.store(true);

    // send shutdown message to the server
    // to close the connection with name_close
    my_data_t shutdownMessage;
    shutdownMessage.commandType = SHUTDOWN_COMMAND;
    MsgSend(server_coid, &shutdownMessage, sizeof(shutdownMessage), NULL, 0);

    name_close(server_coid);
}

void OperatorConsole::run() {
	// create thread to run the messages received from the computer system in parallel
    std::thread receiveThread(&OperatorConsole::receiveMessageFromComputerSystem, this);
    // allow receivethread to run independently
    receiveThread.detach();
    while (true) {
        cout << "Enter command (info == 1, change_level == 2, change_speed == 3, change_position == 4, quit): ";
        string command;
        getline(cin, command);

        if (command == "quit") {
            break;
        }

        int flightID;
        cout << "Enter flight ID: ";
        cin >> flightID;
        // clear input buffer so that extra input doesnt affect multiple reads
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (flightID <= 0) {
            cout << "Invalid Flight ID. Please enter a positive integer." << endl;
            continue;
        }

        my_data_t data;
        data.flightID = flightID;

        if (command == "change_level") {
            int newFlightLevel;
            bool validLevel = false;
            while (!validLevel) {
            	// have to make sure flight levels make sense
                cout << "Enter new flight level (e.g., 150 for FL150): ";
                cin >> newFlightLevel;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (newFlightLevel < 150 || newFlightLevel > 400) {
                    cout << "Invalid flight level. Please enter a value between 150 (FL150) and 400 (FL400)." << endl;
                } else {
                    validLevel = true;
                }
            }
            data.commandType = CHANGE_LEVEL;
            data.arg1 = newFlightLevel * 100; // convert flight level to correct format

            // log command
            logCommand("change_level", flightID, newFlightLevel, 0, 0);
            sendMessageToComputerSystem(data);
            cout << "Flight level change command issued successfully." << endl;


        } else if (command == "change_speed") {
            data.commandType = CHANGE_SPEED;
            cout << "Enter new speed X, Y, Z: ";
            cin >> data.arg1 >> data.arg2 >> data.arg3;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            // log command
            logCommand("change_speed", flightID, data.arg1, data.arg2, data.arg3);
            sendMessageToComputerSystem(data);
            cout << "Speed change command issued successfully." << endl;

        } else if (command == "change_position") {
            data.commandType = CHANGE_POSITION;
            cout << "Enter new position X, Y, Z: ";
            cin >> data.arg1 >> data.arg2 >> data.arg3;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            // log command
            logCommand("change_position", flightID, data.arg1, data.arg2, data.arg3);
            sendMessageToComputerSystem(data);
            cout << "Position change command issued successfully." << endl;

        } else if (command == "info") {
            data.commandType = INFO_COMMAND;
            data.arg1 = data.arg2 = data.arg3 = 0; // no need for arguments
            // log command
            logCommand("info", flightID, 0, 0, 0);
            sendMessageToComputerSystem(data);
        } else {
            cout << "Unknown command." << endl;
        }
    }
}

void OperatorConsole::sendMessageToComputerSystem(const my_data_t& data) {
    // send message to the computer system
	// link console to server with connection id
	// transmits entire data structure encapsulated by my_data_t
	// NULL, 0 cause no reply is expected
    if (MsgSend(server_coid, &data, sizeof(data), NULL, 0) == -1) {
        perror("Error while sending message to computer system");
    }
}

void OperatorConsole::logCommand(const string& command, int flightID, int arg1, int arg2, int arg3) {
    ofstream logfile("operator_log.txt", ios::app);
    if (logfile.is_open()) {
        logfile << "Command: " << command << ", Flight ID: " << flightID;
        if (command == "change_speed") {
            logfile << ", Speed X: " << arg1 << ", Speed Y: " << arg2 << ", Speed Z: " << arg3;
        } else if (command == "change_position") {
            logfile << ", Position X: " << arg1 << ", Position Y: " << arg2 << ", Position Z: " << arg3;
        } else if (command == "change_level") {
            logfile << ", Flight Level: " << arg1;
        }
        logfile << endl;
        logfile.close();
    } else {
        cerr << "Unable to open log file for writing." << endl;
    }
}

void OperatorConsole::receiveMessageFromComputerSystem() {
	// my data holds the received data from the comp sys
    my_data_t receivedData;
    while (!shouldTerminate.load()) {
    	// server coid is connection id to the channel from which messages are received
    	// &receivedata specifies where the msgs received will be stored
        int receiveStatus = MsgReceive(server_coid, &receivedData, sizeof(receivedData), NULL);
        if (receiveStatus == -1) {
            std::cerr << "Error receiving data from the computer system. Exiting thread." << std::endl;
            break;
        }


        switch(receivedData.commandType) {
            case INFO_COMMAND:

                std::cout << "Received augmented information for Aircraft #" << receivedData.flightID << std::endl
                          << "Flight Level: " << receivedData.arg1 << std::endl
                          << "Speed X: " << receivedData.arg2 << ", Y: " << receivedData.arg3 << std::endl;

                break;
            default:
                std::cerr << "Unknown commandType received: " << receivedData.commandType << std::endl;
                break;
        }
    }
}
