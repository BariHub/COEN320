#ifndef COMMUNICATION_SYSTEM_H
#define COMMUNICATION_SYSTEM_H

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include "cTimer.h"
#include <vector>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "Struct_definitions.h"
using namespace std;


class CommSystem{

private:
	int conn_id;

public:
	pthread_t ID;
	CommSystem();
	int send_plane(plane_msg&);
	virtual ~ CommSystem();
};
#endif
