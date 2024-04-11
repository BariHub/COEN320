//The radar system is responsible of requesting data from the aircraft and sending it to the computer system
#ifndef RADAR_H_
#define RADAR_H_

#include <vector>
#include <string>
#include "cTimer.h"
#include "Struct_definitions.h"
#include "Plane.h"
#include <pthread.h>

class Radar {
private:
    int server_coid;
    pthread_t thread_id;
    std::vector<int> airspace;
    std::vector<plane_info> allPlaneData;

    static void* radarRoutine(void* arg);
    void processAirspace(const std::vector<int>& airspace); // obtains info of all planes in the airspace IDs vector
    plane_info getPlaneData(int planeId); // obtains plane info based on ID

public:
    Radar();//constructor
    virtual ~Radar();//destructor
    void getAirspace(); //obtains data from aircraft
    int toComputerSys(compSysMsg data); //sends aircraft data to the computer system
};

#endif // RADAR_H_
