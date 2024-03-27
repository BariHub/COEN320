#include "Radar.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

using namespace std;

class AircraftPositionResponse {
public:
    int x, y, z;
    int flightLevel;
    int speedX, speedY, speedZ;
};

class Radar {
    // tracking planes with radar
    vector<Plane> planes;

public:
    Radar() = default;
    explicit Radar(const vector<Plane>& planes) : planes(planes) {}
    ~Radar() = default;

    // collect plane data with flight ID and data
    vector<pair<int, AircraftPositionResponse>> collectPlaneData();

    // try to find plane with flight ID and data
    bool pingPlane(int flightID, AircraftPositionResponse* out);

    // for sending signal to multiple planes and collecting their response
    vector<AircraftPositionResponse> pingMultiplePlanes(const vector<Plane>& selectedPlanes);

private:
    // for collecting response from a single aircraft
    AircraftPositionResponse signalPlane(const Plane& plane);
};

// iterate over planes tracked, signal them and store flightID
vector<pair<int, AircraftPositionResponse>> Radar::collectPlaneData() {
    vector<pair<int, AircraftPositionResponse>> planeData;
    for (const auto& plane : planes) {
        AircraftPositionResponse response = signalPlane(plane);
        planeData.emplace_back(plane.getFlightId(), response);
    }
    return planeData;
}

// searching for plane using flightID in tracking list
bool Radar::pingPlane(int flightID, AircraftPositionResponse* out) {
    for (const auto& plane : planes) {
        if (plane.getFlightId() == flightID) {
            *out = signalPlane(plane);
            return true;
        }
    }
    return false;
}

// collecting responses from multiple planes
vector<AircraftPositionResponse> Radar::pingMultiplePlanes(const vector<Plane>& selectedPlanes) {
    vector<AircraftPositionResponse> responses;
    for (const auto& plane : selectedPlanes) {
        responses.push_back(signalPlane(plane));
    }
    return responses;
}

// communicate with plane to request its data
AircraftPositionResponse Radar::signalPlane(const Plane& plane) {
    AircraftPositionResponse response;
    // create connection channel with plane
    int connectionID = ConnectAttach(0, 0, plane.getChannelID(), _NTO_SIDE_CHANNEL, 0);
    if (connectionID != -1) {
        PlaneCommandMessage msg = {COMMAND_RADAR_SIGNAL};
        if (MsgSend(connectionID, &msg, sizeof(msg), &response, sizeof(response)) != -1) {
            response.x = plane.getXPosition();
            response.y = plane.getYPosition();
            response.z = plane.getZPosition();
            response.flightLevel = plane.getFlightLevel();
            response.speedX = plane.getXSpeed();
            response.speedY = plane.getYSpeed();
            response.speedZ = plane.getZSpeed();
        }
        // remove connection
        ConnectDetach(connectionID);
    } else {
        cerr << "Failed to attach to plane's channel\n";
    }
    return response;
}
