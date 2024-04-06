#ifndef OPERATOR_CONSOLE_H
#define OPERATOR_CONSOLE_H
#define SHUTDOWN_MSG 0xFF


#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <sys/dispatch.h>

typedef struct _pulse msg_header_t;


typedef struct _my_data {
    msg_header_t hdr;
    int commandType;
    int flightID;
    int arg1, arg2, arg3;
} my_data_t;

class OperatorConsole {
private:
    int server_coid;
    static constexpr const char* ATTACH_POINT = "my_channel";
    std::atomic<bool> shouldTerminate{false};

public:
    OperatorConsole();
    ~OperatorConsole();

    void run();
    void logCommand(const std::string& command, int flightID, int arg1 = 0, int arg2 = 0, int arg3 = 0);
    void changeAirplaneFlightLevel(int flightID, int newFlightLevel);
    void changeAirplaneSpeed(int flightID, int newSpeedX, int newSpeedY, int newSpeedZ);
    void changeAirplanePosition(int flightID, int newX, int newY, int newZ);
    void requestAircraftInfo(int flightID);

    void sendMessageToComputerSystem(const my_data_t& data);
    void receiveMessageFromComputerSystem(my_data_t& data);
};

#endif
