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
    void startTracking();
    void processAirspace(const std::vector<int>& airspace);
    plane_info getPlaneData(int planeId);

public:
    Radar();
    virtual ~Radar();

    void getAirspace();
    int toComputerSys(compSysMsg data);
};

#endif // RADAR_H_
