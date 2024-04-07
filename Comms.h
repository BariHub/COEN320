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
	pthread_t thread_id;
	CommSystem();
	int send_plane(planeMsg&);
	int fromCompSys();
	virtual ~CommSystem();
};
#endif
