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
	vector<Plane> planes;

	CompSys();
	vector<int> viollationVerification();
	int sendBaseInfo();
	int listen();
	int sendToDisplay(compSysToDispMsg Msg);
	int sendToCommSys(planeMsg msg);
	virtual ~CompSys();
};

#endif /* SRC_COMP_SYS_H_ */
