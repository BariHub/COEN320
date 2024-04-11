/*This system is responsible for the communication system to enable the
  operator console to communicate information to the aircraft.*/
#ifndef COMMUNICATION_SYSTEM_H
#define COMMUNICATION_SYSTEM_H

#include <iostream>
#include <pthread.h>
#include <vector>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "Struct_definitions.h"
#include "Plane.h"
#include "cTimer.h"

class CommSystem{

private:
	int serverId;
	int rcvrId;

public:
	pthread_t thread_id; //used for threading
	CommSystem(); //constructor
	int send_plane(MsgToPlane&); //send aircraft a speed change request
	int fromCompSys(); //recieve a speed change request from the operator system through the computer system
	virtual ~CommSystem(); // destructor
};
#endif
