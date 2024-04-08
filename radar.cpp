#include "radar.h"
#include <iostream>
#include <unistd.h>
using namespace std;

#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem"
void* Radar::radarRoutine(void* arg) {
	// thread routine to start tracking
    Radar* radar = static_cast<Radar*>(arg);
    radar->startTracking();
    return NULL;
}

Radar::Radar() : server_coid(-1) {
	// create new thread for radar routine
    if (pthread_create(&thread_id, NULL, radarRoutine, (void*)this) != 0) {
        std::cerr << "Radar: Failed to start radar tracking thread." << std::endl;
    }
}

Radar::~Radar() {

    // wait for thread to finish executing
    pthread_join(thread_id, NULL);
}

// start tracking objects to get info about airspace objects
void Radar::startTracking() {
    while (true) {
        getAirspace();
        // radar checks airspace every 1sec
        sleep(1);
    }
}

int Radar::toComputerSys(compSysMsg data) {
    int server_coid = name_open(COMPUTER_SYSTEM_ATTACH_POINT, 0);
    // try to connect with server
    if (server_coid == -1) {
        cerr << "Radar: Failed to connect with server\n\n";
        return EXIT_FAILURE;
    }
    // try to send message to computer system
    if (MsgSend(server_coid, &data, sizeof(data), nullptr, 0) == -1) {
        cerr << "Radar: Failed to send message\n\n";
        name_close(server_coid);
        return EXIT_FAILURE;
    }
    name_close(server_coid);
    return EXIT_SUCCESS;
}

void Radar::getAirspace() {
    while (true) {
    	sleep(1);
        cout << "Radar: Timer waited 1s" << std::endl;
        airspace = Plane::mPlanesInAirSpace;;

        if (airspace.empty()) {
            cout << "Radar: Airspace empty" << std::endl;
        } else {
            processAirspace(airspace);
        }
    }
}

void Radar::processAirspace(const std::vector<int>& airspace) {
    compSysMsg data;
    data.header.type = 0x01;

    // iterate over each element in airspace
    for (int planeId : airspace) {
        try {
            auto planeData = getPlaneData(planeId);
            allPlaneData.push_back(planeData);
        } catch (const exception& e) {
        	// if error to process plane get next id and exception description
            cerr << "Radar: Failed to process plane " << planeId << ": " << e.what() << std::endl;
            continue;
        }
    }
    // send data to compsys and clear collected data for next
    // data gathering cycle
    if (!allPlaneData.empty()) {
        data.planeList = allPlaneData;
        toComputerSys(data);
        allPlaneData.clear();
    }
}

plane_info Radar::getPlaneData(int planeId) {
	// store plane data
    plane_info planeMsg;

    // connect to server using plane id
    // convert id to string then to c string
    int server_coid = name_open(to_string(planeId).c_str(), 0);
    if (server_coid == -1) {
        throw runtime_error("Radar: Error occurred while attaching to channel");
    }

    msg pos_msg;
    pos_msg.hdr.type = 0x00; //

    if (MsgSend(server_coid, &pos_msg, sizeof(pos_msg), &planeMsg, sizeof(planeMsg)) == -1) {
        name_close(server_coid);
        throw std::runtime_error("Error occurred while sending message");
    }

    name_close(server_coid);
    cout << "Radar: Data of Plane #" << planeMsg.ID << ": Coords(" << planeMsg.PositionX<< ", " << planeMsg.PositionY << ", " << planeMsg.PositionZ << ")" << endl;

    return planeMsg;
}

