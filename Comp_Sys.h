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

class CompSys {

	int serverId;
	int listenId;
	int rcvrId;

public:
	pthread_t thread_id;
	vector<plane_info> planes;

	CompSys();
	vector<int> violationVerification();
	vector<float> NextPos(plane_info, plane_info, int);
	int sendBaseInfo();
	int listen();
	int sendToDisplay(compSysToDispMsg Msg);
	int sendToCommSys(MsgToPlane msg);
	int log(char*, int);
	virtual ~CompSys();
};

#endif /* SRC_COMP_SYS_H_ */
