#include "Comms.h"

using namespace std;

#define COMMUNICATION_SYSTEM_ATTACH_POINT "CommunicationSystem"

//Comms Class constructor
CommSystem::CommSystem(){
	this -> serverId = -1;
	this -> rcvrId = -1;

	if(pthread_create(&thread_id, NULL, commSysStartRoutine, (void *)this) != 0)
		{
			cout<<"Failed to start communication system thread!"<<endl;
		}
}

//msg includes the command as well as the plane id to which this command is to be sent to
int CommSystem::send_plane(plane_msg& msg){
	string plane_ID = to_string(msg.ID);
	msg.header = 0x01; //means msg from compsys to change a parameter
	serverId = name_open(plane_ID,0);
	if(serverId == -1){
		cout<<"Failed to connect to the aircraft!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(serverId, &msg, sizeof(msg),0,0) == -1){
		cout<<"Failed to send the message!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(serverId);
	return EXIT_SUCCESS;
}
int CommSystem::fromCompSys(){
	name_attach_t *attach;
	planeMsg planeMsg;

	if ((attach = name_attach(NULL, COMMUNICATION_SYSTEM_ATTACH_POINT, 0)) == NULL) {
		perror("Error occurred while creating the channel");
	}

	/* Do your MsgReceive's here now with the chid */
	while (true) {
		/* Server will block in this call, until a client calls MsgSend to send a message to
		 * this server through the channel named "myname", which is the name that we set for the channel,
		 * i.e., the one that we stored at ATTACH_POINT and used in the name_attach call to create the channel. */
		rcvrId = MsgReceive(attach->chid, &planeMsg, sizeof(planeMsg), NULL);

		/* In the above call, the received message will be stored at planeMsgwhen the server receives a message.
		 * Moreover, rcvid */

		if (rcvrId == -1) {/* Error condition, exit */
			break;
		}

		if (rcvrId == 0) {/* Pulse received */
			switch (planeMsg.header.code) {
			case _PULSE_CODE_DISCONNECT:
				/*
				 * A client disconnected all its connections (called
				 * name_close() for each name_open() of our name) or
				 * terminated
				 */
				ConnectDetach(planeMsg.header.scoid);
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
		if (planeMsg.header.type == _IO_CONNECT ) {
			MsgReply( rcvrId, EOK, NULL, 0 );
			continue;
		}

		/* Some other QNX IO message was received; reject it */
		if (planeMsg.header.type > _IO_BASE && planeMsg.header.type <= _IO_MAX ) {
			MsgError( rcvrId, ENOSYS );
			continue;
		}

		//make sure header type indicates it comes from computer system
		if (planeMsg.header.type == 0x01) {
			send_Plane(planeMsg);
		}

		MsgReply(rcvrId, EOK, 0, 0);

	}

	/* Remove the name from the space */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}
void* CommSystem::commSysStartRoutine(void *args){
	CommSystem& commSys = *(CommSystem*) args;
	commSys.fromCompSys();
	return NULL;
}
CommSystem::~CommSystem(){}
