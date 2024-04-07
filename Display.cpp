/*
 * Display.cpp
 *
 *  Created on: Apr. 7, 2024
 *      Author: Magym
 */
#include "Display.h"


#define DISPLAY_ATTACH_POINT "Display"

void* dispStartRoutine(void * arg){
	Display& displaySystem = *(Display*) arg;
	displaySystem.DisplayListen();

	displaySystem.gridDisplay(displaySystem.planeList);
	return NULL;
}
Display::Display(){
	this -> rcvrId = -1;
	if(pthread_create(&thread_id, NULL, dispStartRoutine, (void *)this) != 0)
		{
			cout<<"Failed to start computer system thread!"<<endl;
		}
}
int Display::DisplayListen(){
	name_attach_t *attach;
	compSysToDispMsg msg;  //msg recieved from the computer system
	DispSysMsg dispMsg; //list of planes to display
	/* Create a local name (/dev/name/local/...) */
	if ((attach = name_attach(NULL, DISPLAY_ATTACH_POINT, 0)) == NULL) {
		perror("Error occurred while creating the channel");
	}

	/* Do your MsgReceive's here now with the chid */
	while (true) {
		/* Server will block in this call, until a client calls MsgSend to send a message to
		 * this server through the channel named "myname", which is the name that we set for the channel,
		 * i.e., the one that we stored at ATTACH_POINT and used in the name_attach call to create the channel. */
		rcvrId = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		/* In the above call, the received message will be stored at msg when the server receives a message.
		 * Moreover, rcvid */

		if (rcvrId == -1) {/* Error condition, exit */
			break;
		}
		//Copy current aircraft in airspace info to put on display system
		dispMsg.header = msg.header;
		dispMsg.planeList = msg.planeList;



		if (rcvrId == 0) {/* Pulse received */
			switch (msg.header.code) {
			case _PULSE_CODE_DISCONNECT:
				/*
				 * A client disconnected all its connections (called
				 * name_close() for each name_open() of our name) or
				 * terminated
				 */
				ConnectDetach(msg.header.scoid);
				break;
			case _PULSE_CODE_UNBLOCK:
				/*
				 * REPLY blocked client wants to unblock (was hit by
				 * a signal or timed out).  It's up to you if you
				 * reply now or later.
				 */
				break;
			default:
				/*
				 * A pulse sent by one of your processes or a
				 * _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
				 * from the kernel?
				 */
				break;
			}
			continue;
		}

		/* name_open() sends a connect message, must EOK this */
		if (msg.header.type == _IO_CONNECT ) {
			MsgReply( rcvrId, EOK, NULL, 0 );
			continue;
		}

		/* Some other QNX IO message was received; reject it */
		if (msg.header.type > _IO_BASE && msg.header.type <= _IO_MAX ) {
			MsgError( rcvrId, ENOSYS );
			continue;
		}

		//Wait for messages from computer system to display data
		if (msg.header.type == 0x01) {
			planeList = dispMsg.planeList;
			violatingPairs = dispMsg.violatingPlanes;
			cout<<"THE FOLLOWING PAIRS ARE COMMITTING VIOLATIONS IN THE NEXT N SECONDS!"<<endl;
			for (int i = 0; i < violatingPairs.size(); i+=2) {
			        cout << violatingPairs[i]<<" AND "<<violatingPairs[i+1]<<endl;
			    }
		}
		MsgReply(rcvrId, EOK, 0, 0);
		cTimer timer(5,0);

		gridDisplay(planeList);
		timer.waitTimer();
	}

	/* Remove the name from the space */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}

void Display::gridDisplay(vector<plane_info> planeList){
	const int x = 25;
	const int y = 25;
	const int z = 25;
	int xyCellSize = 4000;
	int zCellSize = 1000;


	//creates 3 grids one in X-Y, Y-Z, Z-X
	//XY PLANE
	string gridXY[x][y];

	for(int i=0; i<sizeof(planeList); i++){
		for(int j=0; j<x; j++){
			//check if its in airspace boundaries in x direction
			if(planeList[i].PositionX >= (xyCellSize*j) &&
					planeList[i].PositionX <= (xyCellSize*(j+1))){
				for(int k=0; k<y;k++){
					//now check if its in boundaries in y direction
					if(planeList[i].PositionY>= (xyCellSize*k) &&
										planeList[i].PositionY <= (xyCellSize*(k+1))){
						if(gridXY[j][k] != ""){
							gridXY[j][k] += ",";
						}
						gridXY[j][k] += to_string(planeList[i].ID);
					}
				}
			}
		}
	}

	//YZ PLANE
	string gridYZ[y][z];

	for(int i=0; i<sizeof(planeList); i++){
		for(int j=0; j<y; j++){
			//check if its in airspace boundaries in x direction
			if(planeList[i].PositionY >= (xyCellSize*j) &&
					planeList[i].PositionY <= (xyCellSize*(j+1))){
				for(int k=0; k<z;k++){
					//now check if its in boundaries in y direction
					if(planeList[i].PositionZ >= (zCellSize*k) &&
										planeList[i].PositionZ <= (zCellSize*(k+1))){
						if(gridYZ[j][k] != ""){
							gridYZ[j][k] += ",";
						}
						gridYZ[j][k] += to_string(planeList[i].ID);
					}
				}
			}
		}
	}
	//XZ PLANE
	string gridXZ[x][z];

	for(int i=0; i<sizeof(planeList); i++){
		for(int j=0; j<y; j++){
			//check if its in airspace boundaries in x direction
			if(planeList[i].PositionX >= (xyCellSize*j) &&
					planeList[i].PositionX <= (xyCellSize*(j+1))){
				for(int k=0; k<z;k++){
					//now check if its in boundaries in y direction
					if(planeList[i].PositionZ >= (zCellSize*k) &&
										planeList[i].PositionZ <= (zCellSize*(k+1))){
						if(gridXZ[j][k] != ""){
							gridXZ[j][k] += ",";
						}
						gridXZ[j][k] += to_string(planeList[i].ID);
					}
				}
			}
		}
	}

	//print all three grids
	for(int i =0; i < x; i++){
		for(int j=0; j<y; j++){
			if(gridXY[i][j] == ""){
				cout<<"|";
			}
			else cout<<"|" + gridXY[i][j];
		}
		cout<<endl;
	}
	cout<<endl;

	for(int i =0; i < x; i++){
		for(int j=0; j<z; j++){
			if(gridXZ[i][j] == ""){
				cout<<"|";
			}
			else cout<<"|" + gridXZ[i][j];
		}
		cout<<endl;
	}
	cout<<endl;

	for(int i =0; i < y; i++){
		for(int j=0; j<z; j++){
			if(gridYZ[i][j] == ""){
				cout<<"|";
			}
			else cout<<"|" + gridYZ[i][j];
		}
		cout<<endl;
	}
	cout<<endl;
}


