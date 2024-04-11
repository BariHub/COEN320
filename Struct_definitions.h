/*
 * Struct_definitions.h
 * Here we defined the structs used to hold the data to be transferred between the different systems
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
	int ID,n;
	float positionX, positionY, positionZ, speedx, speedy, speedz;
}MsgToPlane; // planeMsg

typedef struct{
	msg_header_t header;
	int ID,n;
	std::vector<plane_info> planeList;
	std::vector<int> violatingPlanes;
}compSysToDispMsg;

typedef struct{
	msg_header_t header;
	int ID,n;
	float positionx, positiony, positionz, speedx ,speedy ,speedz;
	std::vector<plane_info> planeList;
}compSysMsg;

typedef struct{
	msg_header_t header;
	std::vector<plane_info> planeList;
	std::vector<int> violatingPlanes;
}DispSysMsg;
#endif /* STRUCT_DEFINITIONS_H_ */
