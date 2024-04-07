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
#include "plane.h"
#include "cTimer.h"

class CommSystem{

private:
	int serverId;
	int rcvrId;

public:
	pthread_t ID;
	CommSystem();
	int send_plane(plane_msg&);
	int fromCompSys();
	virtual ~ CommSystem();
};
#endif
