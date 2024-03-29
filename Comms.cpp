#include "Comms.h"

using namespace std;
using namespace CommSystem;

//Comms Class constructor
CommSystem::CommSystem(){

}

//msg includes the command as well as the plane id to which this command is to be sent to
int send_plane(plane_msg& msg){
	string plane_ID = to_string(msg.ID);
	msg.header = 0x01; //means msg from compsys to change a parameter
	conn_id = name_open(plane_ID,0);
	if(conn_id == -1){
		cout<<"Failed to connect to the aircraft!"<<endl;
		return EXIT_FAILURE;
	}
	if(MsgSend(conn_id, &msg, sizeof(msg),0,0) == -1){
		cout<<"Failed to send the message!"<<endl;
		return EXIT_FAILURE;
	}
	name_close(conn_id);
	return EXIT_SUCCESS;
}
CommSystem::~CommSystem(){}
