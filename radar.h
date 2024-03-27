#ifndef RADAR_H
#define RADAR_H

#include <vector>

using namespace std;

class Radar {
public:
    Radar();
    explicit Radar(const vector<Plane>& planes);
    ~Radar();

    vector<pair<int, AircraftPositionResponse>> collectPlaneData();
    bool pingPlane(int flightID, AircraftPositionResponse* out);
    vector<AircraftPositionResponse> signalMultiplePlanes(const vector<Plane>& selectedPlanes);

private:
    vector<Plane> planes;
    AircraftPositionResponse signalPlane(const Plane& plane);
};

#endif
