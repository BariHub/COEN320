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

class CompSys{

	int serverId;
	int listenId;
	int rcvrId;
	int  file;

public:
	pthread_t thread_id;
	vector<plane_info> planes;
	CompSys();
	int sendBaseInfo();
	int listen();
	int sendToDisplay(compSysToDispMsg Msg);
	int sendToCommSys(plane_info msg);
	vector<float> NextPos(plane_info &a1, plane_info &a2, int n);
	vector <int> violationVerification();
	void loggingTheAirspaceSystem(char* buffer, int length);
	virtual ~CompSys();
};

#endif /* SRC_COMP_SYS_H_ */
