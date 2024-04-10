#include "OperatorConsole.h"

OperatorConsole::OperatorConsole(ATC& atcInstance, const std::string& logFilePath) : _atc(atcInstance), server_coid(0) {
    openLogFile(logFilePath);

    if (pthread_create(&thread_id, NULL, &OperatorConsole::userInputThread, this) != 0) {
        std::cerr << "Operator Console: Failed to start input thread\n";
    } else {
        isThreadActive = true;
    }
}

OperatorConsole::~OperatorConsole() {
    if (logFile.is_open()) {
        logFile.close();
    }
    if (isThreadActive) {
        pthread_cancel(thread_id);
        pthread_join(thread_id, NULL);
    }
}

void OperatorConsole::openLogFile(const std::string& path) {
    logFile.open(path, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file at " << path << ".\n";
    } else {
        std::cout << "Log file opened successfully at " << path << ".\n";
    }
}

void OperatorConsole::log(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
        if (!logFile) {
            std::cerr << "Operator Console: Failed to write to log file.\n";
        }
    }
}

static void* OperatorConsole::userInputThread(void* arg) {
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

void OperatorConsole::processUserCommand(const std::string& command) {
    log(command);

    std::istringstream iss(command);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, {});

    if (tokens.empty() || tokens[0] == "exit") {
        return;
    }

    if (tokens.size() < 2) {
        std::cout << "Operator Console: Invalid command format.\n";
        return;
    }

    int planeID = 0;
    try {
        planeID = std::stoi(tokens[1]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument for plane ID." << std::endl;
        return;
    } catch (const std::out_of_range& e) {
        std::cerr << "Plane ID is out of range." << std::endl;
        return;
    }
    std::string commandType = tokens[0];

    if (commandType == "request info") {
        Plane* plane = _atc.findPlaneById(planeID);
        if (!plane) {
            std::cout << "Plane ID " << planeID << " not found.\n";
            return;
        }

        std::cout << "Operator Console: Info for Plane ID " << planeID << ":\n"
                  << "Speed x: " << plane->getSpeedx() << "\n"
                  << "Speed y: " << plane->getSpeedy() << "\n"
                  << "Speed z: " << plane->getSpeedz() << std::endl;
    } else {
        ActionCommand action = stringToActionCommand(commandType);
        if (action == InvalidCommand) {
            std::cout << "Operator Console: Invalid command. Please try again." << std::endl;
            return;
        }
        compsys_msg data;
        data.ID = planeID;

        if (toComputerSys(data) == EXIT_FAILURE) {
            std::cout << "Operator Console: Failed to execute command for plane ID " << planeID << "\n";
        } else {
            std::cout << "Operator Console: Command executed successfully for plane ID " << planeID << "\n";
        }
    }
}

OperatorConsole::ActionCommand OperatorConsole::stringToActionCommand(const std::string& commandString) {
    if (commandString == "change speedx") return AdjustSpeedx;
    if (commandString == "change speedy") return AdjustSpeedy;
    if (commandString == "change speedz") return AdjustSpeedz;
    if (commandString == "request info") return RequestInfo;
    return InvalidCommand;
}

int OperatorConsole::toComputerSys(compsys_msg& data) {
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
