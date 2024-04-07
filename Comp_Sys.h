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
#include "plane.h"
#include "Struct_definitions.h"
#include "Comms.h"

using namespace std;

class CompSys {

	int serverId;
	int listenId;
	int rcvrId;

public:
	vector<Plane> planes;

	CompSys();
	vector<int> viollationVerification();
	int sendCollisionAlert(vector <int> violatingPlanes);
	int sendBaseInfo();
	int listen();
	void operatorRequest();
	void sendExtraInfo(int planeID);
	int sendToDisplay(compSysToDispMsg Msg);
	int sendToCommSys(planeMsg msg);
	virtual ~CompSys();
};

#endif /* SRC_COMP_SYS_H_ */
