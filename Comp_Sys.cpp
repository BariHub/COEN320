 /* Comp_Sys.cpp
 *
 *  Created on: Apr. 5, 2024
 *      Author: Magym
 */

//object to be ran on a separate periodic thread running every 1 second
#include "Comp_Sys.h"
using namespace std;

#define COMPUTER_SYSTEM_ATTACH_POINT "ComputerSystem" // attach point for computer system
#define DISPLAY_ATTACH_POINT "Display" // attach point for display
#define COMMUNICATION_SYSTEM_ATTACH_POINT "CommunicationSystem" // attach point for communication

void* compSysStartRoutine(void *args){ // function to be passed into the thread argument to create thread, used for listening

	CompSys& compSys = *(CompSys*) args;
	compSys.listen();
	return NULL;
}

CompSys::CompSys(){
	this->file = creat( "/data/home/qnxuser/AirspaceLog.txt", S_IRUSR | S_IWUSR | S_IXUSR ); // create the airspace log file
	this -> serverId = -1; // init
	this -> listenId = -1;
	this -> rcvrId = -1;
	plane_info plane;
	plane.ID = 0;
	plane.PositionX = 0.0;
	plane.PositionY =0.0;
	plane.PositionZ =0.0;
	plane.VelocityX =0.0;
	plane.VelocityY =0.0;
	plane.VelocityZ = 0.0;
	fill(planes.begin(), planes.end(), plane);
	if(pthread_create(&thread_id, NULL, compSysStartRoutine, (void *)this) != 0) // create a thread for computer system
	{
		cout<<"Failed to start computer system thread!"<<endl;
	}

}

vector<float> CompSys::NextPos(plane_info &a1, plane_info &a2, int n){
	// calculate the distance between each two planes, and check the distance after n seconds, to predict potential FUTURE collisions/distance
	vector<float> pos1;
	vector<float> pos2;
	vector<float> vel1;
	vector<float> vel2;
	pos1.push_back(a1.PositionX);// position of first plane
	pos1.push_back(a1.PositionY);
	pos1.push_back(a1.PositionZ);

	pos2.push_back(a2.PositionX); // position of second plane
	pos2.push_back(a2.PositionY);
	pos2.push_back(a2.PositionZ);

	vel1.push_back(a1.VelocityX);
	vel1.push_back(a1.VelocityY);
	vel1.push_back(a1.VelocityZ);

	vel2.push_back(a2.VelocityX);
	vel2.push_back(a2.VelocityY);
	vel2.push_back(a2.VelocityZ);

	//Calculate postition after n seconds
	float locA1X = pos1[0] + vel1[0]*n; // calculate distance between two planes, after n seconds
	float locA1Y = pos1[1] + vel1[1]*n;
	float locA1Z = pos1[2] + vel1[2]*n;

	float locA2X = pos2[0] + vel2[0]*n;
	float locA2Y = pos2[1] + vel2[1]*n;
	float locA2Z = pos2[2] + vel2[2]*n;

	vector <float> dist;
	dist.push_back(abs(locA1X-locA2X));
	dist.push_back(abs(locA1Y-locA2Y));
	dist.push_back(abs(locA1Z-locA2Z));
	return dist; // return the vector of distance between each two planes
}
vector <int> CompSys::violationVerification(){ // check if two planes violate each other, do that for every plane in space
	vector <int> violatingPlanes;
	vector <float> distances;

	for (int i=0; i<planes.size();i++){
		for(int j=i+1;j<planes.size();j++){
			//compute current violation
			if(abs(planes[i].PositionX-planes[j].PositionX) < 3000 || abs(planes[i].PositionY-planes[j].PositionY) < 3000 ||
					abs(planes[i].PositionZ-planes[j].PositionZ) <1000) // check if within the boundaries of each other
			{
				violatingPlanes.push_back(planes[i].ID); // push back the ID's of violating pairs
				violatingPlanes.push_back(planes[j].ID);
			}
			//Compute future violations in next n seconds
			distances = NextPos(planes[i], planes[j],n);
			if(distances[0] < 3000 || distances[1] < 3000 || distances[2] < 1000){
				violatingPlanes.push_back(planes[i].ID); // push back potential future collision between two planes
				violatingPlanes.push_back(planes[j].ID);
			}
		}
	}
	return violatingPlanes;
}

int CompSys::listen(){
	compSysMsg planeInfo; // struct containing plane info
	compSysToDispMsg Msg; // message info send from computer system to display
	plane_info planeMsg;
	MsgToPlane toPlaneMsg;
	//toPlaneMsg.header.type = 0X01; // TO INDICATE TO COMMUNICATION SYSTEM THIS IS A MESSAGE FROM COMPUTER SYSTEM
	name_attach_t *attach;


	if ((attach = name_attach(NULL, COMPUTER_SYSTEM_ATTACH_POINT, 0)) == NULL) {
		perror("Error occurred while creating the channel");
	}
	// TAs code
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
			planes = planeInfo.planeList; // list of planes

			//format aircraft data to be sent to the display system
			Msg.header = planeInfo.header;
			Msg.planeList = planes;
			//cout<<"Enter Frequency of violation checks:"<<endl;
			//cin>>n;
			Msg.violatingPlanes = violationVerification(); // return IDs of plane violated airspace distance

			for(int i=0; i<planes.size();i++){
				string logInfo = "Aircraft " + to_string(planes[i].ID) + " - " + to_string(planes[i].PositionX) + " - " + to_string(planes[i].PositionY) + " - "
						+ to_string(planes[i].PositionZ) + " - " + to_string(planes[i].VelocityX) + " - " + to_string(planes[i].VelocityY) + " - "
						+ to_string(planes[i].VelocityZ) + "\n";

				int length = logInfo.length();
				char buffer [length+1];
				strcpy(buffer, logInfo.c_str());
				loggingTheAirspaceSystem(buffer,length); // logging of the airspace
			}
			sendToDisplay(Msg); // send info to display
		}
		//messages from operator system to change the velocity of an aircraft
		else if(planeInfo.header.type == 0x02){
			toPlaneMsg.header.type = 0x02;
			toPlaneMsg.ID = planeInfo.ID;
			if(planeInfo.n != -1){
				n = planeInfo.n;
				Msg.n = n;
				cout<<n<<endl;
			}
			else if(planeInfo.speedx != -1.0)
			{
				toPlaneMsg.ID = planeInfo.ID;
				toPlaneMsg.speedx = planeInfo.speedx;
				toPlaneMsg.speedy = planeInfo.speedy;
				toPlaneMsg.speedz = planeInfo.speedz;
				sendToCommSys(toPlaneMsg);
			}
			else if(planeInfo.speedy != -1.0){
				toPlaneMsg.ID = planeInfo.ID;
				toPlaneMsg.speedx = planeInfo.speedx;
				toPlaneMsg.speedy = planeInfo.speedy;
				toPlaneMsg.speedz = planeInfo.speedz;
				sendToCommSys(toPlaneMsg);
			}
			else if(planeInfo.speedz != -1.0){
				toPlaneMsg.ID = planeInfo.ID;
				toPlaneMsg.speedx = planeInfo.speedx;
				toPlaneMsg.speedy = planeInfo.speedy;
				toPlaneMsg.speedz = planeInfo.speedz;
				sendToCommSys(toPlaneMsg);
			}
		}
		//Process extra display request
		else if(planeInfo.header.type == 0x03){
			Msg.header.type = planeInfo.header.type;
			Msg.ID = planeInfo.ID;
			sendToDisplay(Msg);
		}


		MsgReply(rcvrId, EOK, 0, 0); // send reply that it was received

	}
	/* Remove the name from the channel */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}
int CompSys::sendToDisplay(compSysToDispMsg Msg){ // send necessary information over to display
	if((serverId = name_open(DISPLAY_ATTACH_POINT, 0))==-1){
		cout<<"Failed to create connection with Display system!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(serverId, &Msg, sizeof(Msg),0,0) == -1){
		cout<<"Failed to send message to display system!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(serverId);
	return EXIT_SUCCESS;
}

int CompSys::sendToCommSys(MsgToPlane msg){ // send necessary information to communication system
	if((serverId = name_open(COMMUNICATION_SYSTEM_ATTACH_POINT, 0))==-1){
		cout<<"Failed to create connection with Communication system!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(serverId, &msg, sizeof(msg),0,0) == -1){
		cout<<"Failed to send message to communication system!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(serverId);
	return EXIT_SUCCESS;
}
void CompSys::loggingTheAirspaceSystem(char* buffer, int length){ // log info to file

	int size_written = write(file, buffer, length);
	/* test for error              */
	if( size_written != length ) {
		perror( "Error writing myfile.dat" );

	}

}
CompSys::~CompSys(){}
