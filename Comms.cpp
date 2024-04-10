#include "Comms.h"

using namespace std;

#define COMMUNICATION_SYSTEM_ATTACH_POINT "CommunicationSystem"

void* commSysStartRoutine(void *args){
	CommSystem& commSys = *(CommSystem*) args;
	commSys.fromCompSys();
	return NULL;
}

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
int CommSystem::send_plane(MsgToPlane& msg){
	string Id = to_string(msg.ID);
	char const *plane_ID = Id.c_str();
	msg.header.type = 0x01; //means msg from compsys to plane, change speed
	serverId = name_open(plane_ID, 0);

	if(serverId == -1){
		cout<<"Failed to connect to the aircraft!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(serverId, &msg, sizeof(msg), 0, 0) == -1){
		printf("Failed to send the message to ID: %d!\n", msg.ID);
		return EXIT_FAILURE;
	}

	name_close(serverId); // close connection
	return EXIT_SUCCESS;
}
int CommSystem::fromCompSys(){
	name_attach_t *attach;
	MsgToPlane updateMsg;

	if ((attach = name_attach(NULL, COMMUNICATION_SYSTEM_ATTACH_POINT, 0)) == NULL) {
		perror("Error occurred while creating the channel in Communications");
	}

	/* Do your MsgReceive's here now with the chid */
	while (true) {
		/* Server will block in this call, until a client calls MsgSend to send a message to
		 * this server through the channel named "myname", which is the name that we set for the channel,
		 * i.e., the one that we stored at ATTACH_POINT and used in the name_attach call to create the channel. */
		rcvrId = MsgReceive(attach->chid, &updateMsg, sizeof(updateMsg), NULL);

		/* In the above call, the received message will be stored at planeMsgwhen the server receives a message.
		 * Moreover, rcvid */

		if (rcvrId == -1) {/* Error condition, exit */
			break;
		}

		if (rcvrId == 0) {/* Pulse received */
			switch (updateMsg.header.code) {
			case _PULSE_CODE_DISCONNECT:
				/*
				 * A client disconnected all its connections (called
				 * name_close() for each name_open() of our name) or
				 * terminated
				 */
				ConnectDetach(updateMsg.header.scoid);
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
		if (updateMsg.header.type == _IO_CONNECT ) {
			MsgReply( rcvrId, EOK, NULL, 0 );
			continue;
		}

		/* Some other QNX IO message was received; reject it */
		if (updateMsg.header.type > _IO_BASE && updateMsg.header.type <= _IO_MAX ) {
			MsgError( rcvrId, ENOSYS );
			continue;
		}

		//make sure header type indicates it comes from computer system
		if (updateMsg.header.type == 0x02) {
			send_plane(updateMsg);
		}

		MsgReply(rcvrId, EOK, 0, 0);
	}

	/* Remove the name from the space */
	name_detach(attach, 0);
	return EXIT_SUCCESS;
}

CommSystem::~CommSystem(){}
