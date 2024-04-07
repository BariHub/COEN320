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
vector<float> CompSys::NextPos(Plane &a1, Plane &a2){

	vector <float> a1Pos = a1.getPosition();
	vector <float> a2Pos = a2.getPosition();

	vector <float> a1Vel = a1.getVelocity();
	vector <float> a2Vel = a2.getVelocity();

	//Calculate postition after 60s
	float locA1X = a1Pos[0] + a1Vel[0]*60;
	float locA1Y = a1Pos[1] + a1Vel[1]*60;
	float locA1Z = a1Pos[2] + a1Vel[2]*60;

	float locA2X = a2Pos[0] + a2Vel[0]*60;
	float locA2Y = a2Pos[1] + a2Vel[1]*60;
	float locA2Z = a2Pos[2] + a2Vel[2]*60;

	vector <float> dist;
	dist.push_back(abs(locA1X-locA2X));
	dist.push_back(abs(locA1Y-locA2Y));
	dist.push_back(abs(locA1Z-locA2Z));
	return dist;
}
vector <int> CompSys::viollationVerification(){
	vector <int> volatingPlanes;
	vector <float> distances;
	for (int i=0; i<planes.size();i++){
		for(int j=i+1;j<planes.size();j++){
			//compute current violation
			if(abs(planes[i].mPosition[0]-planes[j].mPosition[0]) < 3000 || abs(planes[i].mPosition[1]-planes[j].mPosition[1]) < 3000 || abs(planes[i].mPosition[2]-planes[j].mPosition[2]) <1000){
							violatingPlanes.push_back(planes[i].mID);
							violatingPlanes.push_back(planes[j].mID);
			}
			//Compute future violations in next 1 minute
			distances = NextPos(planes[i], planes[j]);
			if(distances[0] < 3000 || distances[1] < 3000 || distances[2] <1000){
				violatingPlanes.push_back(planes[i].mID);
				violatingPlanes.push_back(planes[j].mID);
			}
		}
	}
	return violatinPlanes;
}
//sends alert to display system
int CompSys::sendCollisionAlert(compSysToDispMsg Msg){
	//CONNECTION TO SEND INFO TO THE DISPLAY SYSTMEM
	if((serverId = name_open(DISPLAY_ATTACH_POINT,0)) ==-1){
		cout<<"Couldn't create channel!"<<endl;
		exit(EXIT_FAILURE);
	}
	if(MsgSend(server_id, &Msg, sizeof(Msg),0,0) == -1){

		cout<<"Failed to send message!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(server_id);
	return  EXIT_SUCCESS;
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
void* CompSys::compSysStartRoutine(void *args){
	CompSys& compSys = *(CompSys*) args;
	compSys.listen();
	return NULL;
}


