#ifndef OPERATORCONSOLE_H
#define OPERATORCONSOLE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <pthread.h>


class OperatorConsole {
public:
    enum ActionCommand {
        AdjustSpeedx,
        AdjustSpeedy,
        AdjustSpeedz,
        RequestInfo,
        InvalidCommand
    };

private:
    ATC& _atc;
    std::ofstream logFile;
    bool isThreadActive;
    pthread_t thread_id;
    int server_coid;

    void openLogFile(const std::string& path);
    static void* userInputThread(void* arg);
    void processUserCommand(const std::string& command);
    ActionCommand stringToActionCommand(const std::string& commandString);
    int toComputerSys(compsys_msg& data);

public:
    OperatorConsole(ATC& atcInstance, const std::string& logFilePath);
    ~OperatorConsole();

    void log(const std::string& message);
};

#endif // OPERATORCONSOLE_H
