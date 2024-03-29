/*
 * Struct_definitions.h
 *
 *  Created on: Mar. 28, 2024
 *      Author: Magym
 */

#ifndef STRUCT_DEFINITIONS_H_
#define STRUCT_DEFINITIONS_H_

typedef struct{
	msg_header_t header;
	int ID, altitude, speedx ,speedy ,speedz, positionz ,positionx;
}plane_msg;

#endif /* STRUCT_DEFINITIONS_H_ */
