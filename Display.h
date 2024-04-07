/*
 * Display.h
 *
 *  Created on: Apr. 7, 2024
 *      Author: Magym
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
	int rcvrId;

public:
	pthread_t thread_id;
	vector<plane_info> planeList;
	vector<int> violatingPairs;
	Display();
	int DisplayListen();
	void gridDisplay(vector<plane_info> planeList);
	virtual ~Display();

};

#endif /* SRC_DISPLAY_H_ */
