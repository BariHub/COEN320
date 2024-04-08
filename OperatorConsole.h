#ifndef OPERATORCONSOLE_H
#define OPERATORCONSOLE_H

#include <string>
#include <vector>
#include <pthread.h>

struct compsys_msg;
class ATC;

enum actionCommand {
    adjustSpeed,
    adjustFL,
    adjustPosition,
    invalidCommand
};

class OperatorConsole {
public:
    OperatorConsole(ATC atc);
    ~OperatorConsole();

    void getCommands();

private:
    ATC atc;
    int server_coid;
    pthread_t thread_id;

    int toComputerSys(compsys_msg data);
    void applyCommandToPlane(compsys_msg& data, const std::string& command, int amount);
    void processCommandsForPlane();
    static void* operator_console_start_routine(void *arg);

    static actionCommand stringToActionCommand(const std::string& inString);
};

#endif // OPERATORCONSOLE_H
