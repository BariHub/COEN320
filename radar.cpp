#include "Radar.h"
#include <iostream>
#include <cmath>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

using std::cerr;
using std::cout;
using std::endl;
using std::vector;


Radar::Radar() = default;

Radar::Radar(const std::vector<Plane>& planes) : planes(planes) {}

Radar::~Radar() = default;

// to collect data for all planes monitored by radar
std::vector<std::pair<int, AircraftPositionResponse>> Radar::collectPlaneData() {
    std::vector<std::pair<int, AircraftPositionResponse>> planeData;
    for (const auto& plane : planes) {
        // signal plane and get its response
        AircraftPositionResponse response = signalPlane(plane);
        // store response along with plane's flight ID
        planeData.emplace_back(plane.getFlightId(), response);
    }
    return planeData;
}

// collect data from a specific plane
bool Radar::pingPlane(int flightID, AircraftPositionResponse* out) {
    // check if plane's flight ID matches requested ID
    for (const auto& plane : planes) {
        if (plane.getFlightId() == flightID) {
            // signal plane and get its response
            *out = signalPlane(plane);
            return true;
        }
    }
    // if requested id is not found
    return false;
}

// collect data for a specific subset of planes and get their responses
std::vector<AircraftPositionResponse> Radar::pingMultiplePlanes(const std::vector<Plane>& selectedPlanes) {
    std::vector<AircraftPositionResponse> responses;
    for (const auto& plane : selectedPlanes) {
        // signal each plane to get its response
        responses.push_back(signalPlane(plane));
    }
    return responses;
}

// signal a plane and get its response
AircraftPositionResponse Radar::signalPlane(const Plane& plane) {
    AircraftPositionResponse response;
    // connect to the plane's channel
    int connectionID = ConnectAttach(0, 0, plane.getChannelID(), _NTO_SIDE_CHANNEL, 0);
    // check if connection is successful
    if (connectionID != -1) {
        // create a message to signal the plane
        PlaneCommandMessage msg = {COMMAND_RADAR_SIGNAL};
        // send the message and receive its response
        if (MsgSend(connectionID, &msg, sizeof(msg), &response, sizeof(response)) != -1) {
            // get responses for position, FL, and speed
            response.x = plane.getXPosition();
            response.y = plane.getYPosition();
            response.z = plane.getZPosition();
            response.flightLevel = plane.getFlightLevel();
            response.speedX = plane.getXSpeed();
            response.speedY = plane.getYSpeed();
            response.speedZ = plane.getZSpeed();
        }
        // detach from channel
        ConnectDetach(connectionID);
    } else {
        cerr << "Failed to attach to plane's channel\n";
    }
    return response;
}
