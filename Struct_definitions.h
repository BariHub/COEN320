/*
 * Struct_definitions.h
 *
 *  Created on: Mar. 28, 2024
 *      Author: Magym
 */

#ifndef STRUCT_DEFINITIONS_H_
#define STRUCT_DEFINITIONS_H_
#include "Plane.h"
#include <vector>

typedef struct _pulse msg_header_t;

typedef struct _plane_info{
	int ID;
	float PositionX, PositionY, PositionZ, VelocityX, VelocityY, VelocityZ;
} plane_info;
typedef struct _my_msg {
	msg_header_t hdr;
} msg;

typedef struct{
	msg_header_t header;
	int ID;
	float positionX, positionY, positionZ, speedx, speedy, speedz;
}MsgToPlane; // planeMsg

typedef struct{
	msg_header_t header;
	int ID;
	std::vector<plane_info> planeList;
	std::vector<int> violatingPlanes;
}compSysToDispMsg;

typedef struct{
	msg_header_t header;
	int ID;
	float positionx, positiony, positionz, speedx ,speedy ,speedz;
	std::vector<plane_info> planeList;
}compSysMsg;

typedef struct{
	msg_header_t header;
	std::vector<plane_info> planeList;
	std::vector<int> violatingPlanes;
}DispSysMsg;



#endif /* STRUCT_DEFINITIONS_H_ */
