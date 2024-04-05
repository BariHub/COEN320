#ifndef RADAR_H
#define RADAR_H

#include <vector>

class Plane;

struct AircraftPositionResponse {
    int x;
    int y;
    int z;
    int flightLevel;
    int speedX;
    int speedY;
    int speedZ;
};

struct PlaneCommandMessage {
    int command;
};

class Radar {
public:
    Radar();
    explicit Radar(const std::vector<Plane>& planes);
    ~Radar();

    std::vector<std::pair<int, AircraftPositionResponse>> collectPlaneData();
    bool pingPlane(int flightID, AircraftPositionResponse* out);
    std::vector<AircraftPositionResponse> pingMultiplePlanes(const std::vector<Plane>& selectedPlanes);

private:
    std::vector<Plane> planes;
    AircraftPositionResponse signalPlane(const Plane& plane);
};

#endif // RADAR_H
