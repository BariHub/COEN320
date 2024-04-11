/*
 * Display.h
 * This system is responsible of all outputs to the console to display the airspace at all times
 * and the additional requested information for the operator.
 */
#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

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
#include "Comp_Sys.h"
#include "Comms.h"
#include "cTimer.h"

using namespace std;

class Display{
	// only recieves no message sending
	int rcvrId;

public:
	pthread_t thread_id; //thread id for thread creation
	vector<plane_info> planeList; //list of planes in the airspace
	vector<int> violatingPairs; //pairs of aircraft violating the distance requirements in the airspace
	Display();//constructor
	int DisplayListen();//process running on thread waiting for data from computer system
	void gridDisplay(vector<plane_info> planeList); //displays airspace
	virtual ~Display(); //destructor

};

#endif /* SRC_DISPLAY_H_ */
