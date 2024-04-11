#include "radar.h"
#include <iostream>
#include <unistd.h>
using namespace std;
//computer system attach point to communicate with the computer system
#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem"
//start routine for radar threads
void* Radar::radarRoutine(void* arg) {
	// thread routine to start tracking
    Radar& radar = *(Radar*) arg;
    radar.getAirspace(); //start obtaining the airspace
    return NULL;
}
//constructor
Radar::Radar() : server_coid(-1) {
	// create new thread for radar
    if (pthread_create(&thread_id, NULL, radarRoutine, (void*)this) != 0) {
        std::cerr << "Radar: Failed to start radar tracking thread." << std::endl;
    }
}
//destructor
Radar::~Radar() {}

int Radar::toComputerSys(compSysMsg data) {
	//open connection with computer system via attach point
    int server_coid = name_open(COMPUTER_SYSTEM_ATTACH_POINT, 0);
    // try to connect with server
    if (server_coid == -1) {
        cerr << "Radar: Failed to connect with server\n\n";
        return EXIT_FAILURE;
    }
    // try to send message to computer system
    if (MsgSend(server_coid, &data, sizeof(data), nullptr, 0) == -1) {//send message to the computer system
        cerr << "Radar: Failed to send message\n\n";
        name_close(server_coid);
        return EXIT_FAILURE;
    }
    name_close(server_coid);
    return EXIT_SUCCESS;
}

void Radar::getAirspace() {
    while (true) {
    	cTimer timer(1,0); //timer of 1 second to evaluate airspace again
        airspace = Plane::mPlanesInAirSpace;

        if (airspace.empty()) {
            cout << "Radar: Airspace empty" << std::endl; //message to indicate that the airspace is empty
        } else {

            processAirspace(airspace); // process the airspace data to
        }
        timer.waitTimer();//wait for time to elapse and go again
    }
}

void Radar::processAirspace(const std::vector<int>& airspace) {
    compSysMsg data;
    data.header.type = 0x01;//indicate to computer system that the radar wants to update it with the new information

    // iterate over each element in airspace
    for (int planeId : airspace) {
        try {
            auto planeData = getPlaneData(planeId); //obtain plane data
            allPlaneData.push_back(planeData); //save plane data in a vector
        } catch (const exception& e) {
        	// if error to process plane get next id and exception description
            cerr << "Radar: Failed to process plane " << planeId << ": " << e.what() << std::endl;
            continue;
        }
    }
    // send data to compsys and clear collected data for next
    // data gathering cycle
    if (!allPlaneData.empty()) {
        data.planeList = allPlaneData; //save plane data vector in the computer system message struct
        toComputerSys(data); //send data of aircraft to computer system
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
    pos_msg.hdr.type = 0x00; //Pulse to indicate to aircraft the radar needs its data

    if (MsgSend(server_coid, &pos_msg, sizeof(pos_msg), &planeMsg, sizeof(planeMsg)) == -1) { //send a request of data to be stored in planeMsg TO AIRCRAFT
        name_close(server_coid);
        throw std::runtime_error("Error occurred while sending message");
    }

    name_close(server_coid);
    //cout << "Radar: Data of Plane #" << planeMsg.ID << ": Coords(" << planeMsg.PositionX<< ", " << planeMsg.PositionY << ", " << planeMsg.PositionZ << ")" << endl;

    return planeMsg;
}

