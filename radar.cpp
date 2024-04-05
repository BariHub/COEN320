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

// signal one plane and get its response
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
        } else {
            cerr << "Failed to send signal to the plane" << endl;
            response.x = response.y = response.z = 0.0;
            response.flightLevel = 0;
            response.speedX = response.speedY = response.speedZ = 0;
        }
        // detach from channel
        ConnectDetach(connectionID);
    } else {
        cerr << "Failed to attach to plane's channel" << endl;
        response.x = response.y = response.z = 0.0;
        response.flightLevel = 0;
        response.speedX = response.speedY = response.speedZ = 0;
    }
    return response;
}

// collect and send data for all planes monitored by radar
void Radar::collectAndSendAircraftData() {
    for (const auto& plane : planes) {
        AircraftPositionResponse response = signalPlane(plane);

        AircraftData data;
        data.flightId = plane.getFlightId();
        data.x = response.x;
        data.y = response.y;
        data.z = response.z;
        data.flightLevel = response.flightLevel;
        data.speedX = response.speedX;
        data.speedY = response.speedY;
        data.speedZ = response.speedZ;

        // send aircraft data
        if (!sendAircraftData(data)) {

            cerr << "Failed to send aircraft data for plane with ID: " << plane.getFlightId() << endl;
        }
    }
}

// send aircraft data via message passing to computer system
bool Radar::sendAircraftData(const AircraftData& data) {
    int connectionID = ConnectAttach(0, 0, CHANNEL_ID, _NTO_SIDE_CHANNEL, 0);
    if (connectionID != -1) {
        if (MsgSend(connectionID, &data, sizeof(data), NULL, 0) != -1) {
            ConnectDetach(connectionID);
            return true;
        } else {
            cerr << "Failed to send aircraft data" << endl;
            ConnectDetach(connectionID);
            return false;
        }
    } else {
        cerr << "Failed to attach to computer system channel" << endl;
        return false;
    }
}
