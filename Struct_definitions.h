/*
 * Struct_definitions.h
 *
 *  Created on: Mar. 28, 2024
 *      Author: Magym
 */

#ifndef STRUCT_DEFINITIONS_H_
#define STRUCT_DEFINITIONS_H_
#include "plane.h"

typedef struct _pulse msg_header_t;

typedef struct{
	msg_header_t header;
	int ID, altitude, speedx ,speedy ,speedz, positionz ,positionx;
}planeMsg;

typedef struct{
	msg_header_t header;
	std:: vector <Plane> planeList;
	std::vector<int> violatingPlanes;

}compSysToDispMsg;

typedef struct{
	msg_header_t header;
	int ID, altitude, speedx ,speedy ,speedz, positionz ,positionx;
	std:: vector <Plane> planeList;
}compSysMsg;

#endif /* STRUCT_DEFINITIONS_H_ */
