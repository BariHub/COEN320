/*
 * Comp_Sys.cpp
 *
 *  Created on: Apr. 5, 2024
 *      Author: Magym
 */

//object to be ran on a separate periodic thread running every 1 second
#include "Comp_Sys.h"
using namespace std;

#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem"
#define DISPLAY_ATTACH_POINT "Display"
#define COMMUNICATION_SYSTEM_ATTACH_POINT "CommunicationSystem"

void* compSysStartRoutine(void *args){
	CompSys& compSys = *(CompSys*) args;
	compSys.listen();
	return NULL;
}

CompSys::CompSys(){
	this -> serverId = -1;
	this -> listenId = -1;
	this -> rcvrId = -1;
	if(pthread_create(&thread_id, NULL, compSysStartRoutine, (void *)this) != 0)
	{
		cout<<"Failed to start computer system thread!"<<endl;
	}

}
//calculate position in 1 minute from current moment
vector<float> CompSys::NextPos(plane_info &a1, plane_info &a2, int n){

	vector<float> pos1;
	vector<float> pos2;
	vector<float> vel1;
	vector<float> vel2;
	pos1.push_back(a1.PositionX);
	pos1.push_back(a1.PositionY);
	pos1.push_back(a1.PositionZ);

	pos2.push_back(a2.PositionX);
	pos2.push_back(a2.PositionY);
	pos2.push_back(a2.PositionZ);

	vel1.push_back(a1.VeloctyX);
	vel1.push_back(a1.VeloctyY);
	vel1.push_back(a1.VeloctyZ);

	vel2.push_back(a2.VeloctyX);
	vel2.push_back(a2.VeloctyY);
	vel2.push_back(a2.VeloctyZ);

	//Calculate postition after 60s
	float locA1X = pos1[0] + vel1[0]*n;
	float locA1Y = pos1[1] + vel1[1]*n;
	float locA1Z = pos1[2] + vel1[2]*n;

	float locA2X = pos2[0] + vel2[0]*n;
	float locA2Y = pos2[1] + vel2[1]*n;
	float locA2Z = pos2[2] + vel2[2]*n;

	vector <float> dist;
	dist.push_back(abs(locA1X-locA2X));
	dist.push_back(abs(locA1Y-locA2Y));
	dist.push_back(abs(locA1Z-locA2Z));
	return dist;
}
vector <int> CompSys::violationVerification(){
	vector <int> volatingPlanes;
	vector <float> distances;

	for (int i=0; i<planes.size();i++){
		for(int j=i+1;j<planes.size();j++){
			//compute current violation
			if(abs(planes[i].PositionX-planes[j].PositionX) < 3000 || abs(planes[i].PositionY-planes[j].PositionY) < 3000 ||
					abs(planes[i].PositionZ-planes[j].PositionZ) <1000)
			{
				violatingPlanes.push_back(planes[i].mID);
				violatingPlanes.push_back(planes[j].mID);
			}
			//Compute future violations in next 1 minute
			distances = NextPos(planes[i], planes[j]);
			if(distances[0] < 3000 || distances[1] < 3000 || distances[2] < 1000){
				violatingPlanes.push_back(planes[i].mID);
				violatingPlanes.push_back(planes[j].mID);
			}
		}
	}
	return violatinPlanes;
}

int CompSys::listen(){
	compSysMsg planeInfo;
	compSysToDispMsg Msg;
	planeMsg planeMsg;
	planeMsg.header.type = 0X01; // TO INDICATE TO COMMUNICATION SYSTEM THIS IS A MESSAGE FROM COMPUTER SYSTEM
	name_attach_t *attach;
	my_data_t msg;

	if ((attach = name_attach(NULL, COMPUTER_SYSTEM_ATTACH_POINT, 0)) == NULL) {
		perror("Error occurred while creating the channel");
	}

	/* Do your MsgReceive's here now with the chid */
	while (true) {

		rcvrId = MsgReceive(attach->chid, &planeInfo, sizeof(planeInfo), NULL);

		/* In the above call, the received message will be stored at msg when the server receives a message.
		 * Moreover, rcvid */

		if (rcvrId == -1) {/* Error condition, exit */
			break;
		}

		if (rcvrId == 0) {/* Pulse received */
			switch (planeInfo.header.code) {
			case _PULSE_CODE_DISCONNECT:
				/*
				 * A client disconnected all its connections (called
				 * name_close() for each name_open() of our name) or
				 * terminated
				 */
				ConnectDetach(planeInfo.header.scoid);
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
		if (planeInfo.header.type == _IO_CONNECT ) {
			MsgReply( rcvrId, EOK, NULL, 0 );
			continue;
		}

		/* Some other QNX IO message was received; reject it */
		if (planeInfo.header.type > _IO_BASE && planeInfo.header.type <= _IO_MAX ) {
			MsgError( rcvrId, ENOSYS );
			continue;
		}

		//messages from radar first
		if (planeInfo.header.type == 0x01) {
			//save planes info in planes vector on comp system
			planes = planeInfo.planeList;

			//format aircraft data to be sent to the display system
			Msg.header = planeInfo.header;
			Msg.planeList = planes;
			Msg.violatingPlanes = viollationVerification();

			sendToDisplay(Msg);
		}
		//messages from operator system
		else if(planeInfo.header.type == 0x02){

			planeMsg.ID = planeInfo.ID;
			planeMsg.altitude = planeInfo.altitude;
			planeMsg.positionx = planeInfo.positionx;
			planeMsg.positionz = planeInfo.positionz;
			planeMsg.speedx = planeInfo.speedx;
			planeMsg.speedy = planeInfo.speedy;
			planeMsg.speedz = planeInfo.speedz;
			sendToCommSys(planeMsg);
		}

		MsgReply(rcvrId, EOK, 0, 0);

	}
	/* Remove the name from the space */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}
int CompSys::sendToDisplay(compSysToDispMsg Msg){
	if((serverId = name_open(DISPLAY_ATTACH_POINT, 0))==-1){
		cout<<"Failed to create connection with Display system!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(serverId, &msg, sizeof(msg),0,0) == -1){
		cout<<"Failed to send message to display system!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(serverId);
	return EXIT_SUCCESS;
}

int CompSys::sendToCommSys(planeMsg msg){
	if((serverId = name_open(COMMUNICATION_ATTACH_POINT, 0))==-1){
		cout<<"Failed to create connection with Display system!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(serverId, &msg, sizeof(msg),0,0) == -1){
		cout<<"Failed to send message to display system!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(serverId);
	return EXIT_SUCCESS;
}

