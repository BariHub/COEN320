/*
 * Comp_Sys.h
 *
 *  Created on: Apr. 5, 2024
 *      Author: Magym
 */

#ifndef SRC_COMP_SYS_H_
#define SRC_COMP_SYS_H_

#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "Plane.h"
#include "Struct_definitions.h"
#include "Comms.h"

using namespace std;

class CompSys{ // computer system

	int serverId; // id of server
	int listenId;
	int rcvrId;
	int file;
	int n; // number of seconds

public:
	pthread_t thread_id; // thread id
	vector<plane_info> planes; // information of plane
	CompSys();
	int sendBaseInfo();
	int listen(); // listen for messages
	int sendToDisplay(compSysToDispMsg Msg); // send information to display
	int sendToCommSys(MsgToPlane msg); // send info to communication system
	vector<float> NextPos(plane_info &a1, plane_info &a2, int n); // calculate the distance between two planes, and its future positions after n seconds
	vector <int> violationVerification(); // calculate violation (if any) of current time, and the next n number of seconds if any
	void loggingTheAirspaceSystem(char* buffer, int length); // log the planes in airspace
	virtual ~CompSys();
};

#endif /* SRC_COMP_SYS_H_ */
