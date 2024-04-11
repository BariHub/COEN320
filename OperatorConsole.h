/*
 * Display.h
 * This system is responsible of all outputs to the console to display the airspace at all times
 * and the additional requested information for the operator.
 */
#ifndef OPERATORCONSOLE_H
#define OPERATORCONSOLE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <pthread.h>
#include "Struct_definitions.h"
#include "Plane.h"
#include "Comp_Sys.h"
#include "Comms.h"
#include "AirTrafficControl.h"

class OperatorConsole {
private:
    int logFile; //log file for commands
    pthread_t thread_id; //for thread creation
    int server_coid; //sending commands

    int toComputerSys(compSysMsg& data); //send commands to the computer system
public:
    OperatorConsole();//constructor
    ~OperatorConsole();//destructor
    void log(const std::string& message);//logs commands in log file
    void processUserCommand(const std::string& command); //determines how the user commands are to be sent
};

#endif // OPERATORCONSOLE_H
