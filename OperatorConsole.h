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
public:
    enum ActionCommand {
        AdjustSpeedx,
        AdjustSpeedy,
        AdjustSpeedz,
        InvalidCommand
    };

private:
    int logFile;
    bool isThreadActive;
    pthread_t thread_id;
    int server_coid;

    void openLogFile(const std::string& path);
    int toComputerSys(compSysMsg& data);

public:
    OperatorConsole();
    ~OperatorConsole();
    void log(const std::string& message);
    void processUserCommand(const std::string& command);
};

#endif // OPERATORCONSOLE_H
