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
    std::vector<planes_information> allPlaneData;

    static void* radarRoutine(void* arg);
    void startTracking();
    void processAirspace(const std::vector<int>& airspace);
    planes_information getPlaneData(int planeId);

public:
    Radar();
    virtual ~Radar();

    void getAirspace();
    int toComputerSys(compsys_msg data);
};

#endif // RADAR_H_
