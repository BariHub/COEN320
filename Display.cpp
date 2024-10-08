#include "Display.h"


#define DISPLAY_ATTACH_POINT "Display"
//start routine for display threads
void* dispStartRoutine(void * arg){
	Display& displaySystem = *(Display*) arg;
	displaySystem.DisplayListen(); //listens for information from the computer system
	displaySystem.gridDisplay(displaySystem.planeList); //displays airspace
	return NULL;
}
//constructor
Display::Display(){
	//initialization
	this -> rcvrId = -1;
	plane_info plane;
	plane.ID = 0;
	plane.PositionX = 0.0;
	plane.PositionY =0.0;
	plane.PositionZ =0.0;
	plane.VelocityX =0.0;
	plane.VelocityY =0.0;
	plane.VelocityZ = 0.0;
	fill(planeList.begin(), planeList.end(),plane);
	fill(violatingPairs.begin(), violatingPairs.end(), 0);
	//creating thread according to implementation from tutorial
	if(pthread_create(&thread_id, NULL, dispStartRoutine, (void *)this) != 0)
		{
			cout<<"Failed to start computer system thread!"<<endl;
		}
}
int Display::DisplayListen(){
	name_attach_t *attach;
	compSysToDispMsg msg;  //msg recieved from the computer system
	DispSysMsg dispMsg; //list of planes to display
	// create channel to recieve messages from other systems
	if ((attach = name_attach(NULL, DISPLAY_ATTACH_POINT, 0)) == NULL) {
		perror("Error occurred while creating the channel");
	}


	while (true) {

		rcvrId = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);



		if (rcvrId == -1) {/* Error condition, exit */
			break;
		}
		//Copy current aircraft in airspace info to put on display system
		dispMsg.header = msg.header;
		dispMsg.planeList = msg.planeList;


		//Line 57 to 95 is from tutorial  code to treat any errors in the message reception
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
		if (msg.header.type == 0x01) { //header indicating message reception from comp system to display a violation that will occur
			planeList = msg.planeList;
			violatingPairs = msg.violatingPlanes;
			int N = msg.n;
			cout<<"THE FOLLOWING PAIRS ARE COMMITTING VIOLATIONS IN THE NEXT " << N << " SECONDS!"<<endl;
			for (int i = 0; i < violatingPairs.size(); i+=4) { //to be verified
			    cout << violatingPairs[i]<<" AND "<<violatingPairs[i+1]<<endl;
			    violatingPairs[i]=0;
			    violatingPairs[i+1]=0;
			    }
		}
		if(msg.header.type == 0x03){ // header indicating the operator needs more information about a specific aircraft
			int ID = msg.ID;

			for(int i = 0; i < msg.planeList.size(); i++){// find aircraft with given id and print info on display
				if(msg.planeList[i].ID == ID){
					cout<<"The operator requested more data about Aircraft" << ID
							<<": Coords(" << msg.planeList[i].PositionX << ", " << msg.planeList[i].PositionY << ", " << msg.planeList[i].PositionZ << ") And velocity :("
							<<msg.planeList[i].VelocityX <<", "<< msg.planeList[i].VelocityY <<", "<< msg.planeList[i].VelocityZ <<")."<< endl;
				}
			}
		}
		MsgReply(rcvrId, EOK, 0, 0); //reply to computer system that message is recieved properly
		cTimer timer(5,0); //5 seconds timer to display grid every 5 seconds

		gridDisplay(planeList); // dispaly airspace
		timer.waitTimer();
	}

	/* Remove the name from the space */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}

void Display::gridDisplay(vector<plane_info> planeList){
	const int x = 25; // x dimension scaled for console display
	const int y = 25;// y dimension scaled for console display
	const int z = 40;// z dimension scaled for console display
	int xyCellSize = 4000; //cellsize to mimic scale
	int zCellSize = 1000; //cellsize to mimic scale


	//creates 3 grids one in X-Y, Y-Z, Z-X
	//XY PLANE
	string gridXY[x][y] = {" "};//initialize XY grid

	for(int i=0; i<planeList.size(); i++){
		for(int j=0; j<x; j++){
			//check if its in airspace boundaries in x direction
			if(planeList[i].PositionX >= (xyCellSize*j) && planeList[i].PositionX <= (xyCellSize*(j+1))){
				for(int k=0; k<y;k++){
					//now check if its in boundaries in y direction
					if(planeList[i].PositionY >= (xyCellSize*k) && planeList[i].PositionY <= (xyCellSize*(k+1))){
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
	string gridYZ[y][z]={" "};

	for(int i=0; i<planeList.size(); i++){
		for(int j=0; j<y; j++){
			//check if its in airspace boundaries in x direction
			if(planeList[i].PositionY >= (xyCellSize*j) &&
					planeList[i].PositionY <= (xyCellSize*(j+1))){
				for(int k=15; k<z;k++){
					//now check if its in boundaries in y direction
					if(planeList[i].PositionZ >= (zCellSize*k) && planeList[i].PositionZ <= (zCellSize*(k+1))){
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
	string gridXZ[x][z]={" "};

	for(int i=0; i<planeList.size(); i++){
		for(int j=0; j<x; j++){
			//check if its in airspace boundaries in x direction
			if(planeList[i].PositionX >= (xyCellSize*j) && planeList[i].PositionX <= (xyCellSize*(j+1))){
				for(int k=15; k<z;k++){
					//now check if its in boundaries in y direction
					if(planeList[i].PositionZ >= (zCellSize*k) && planeList[i].PositionZ <= (zCellSize*(k+1))){
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
	std::cout << "X-Y Axis.\n";
	for(int i =0; i < x; i++){
		for(int j=0; j<y; j++){
			if(gridXY[i][j] == ""){
				cout<<".";
			}
			else cout<<gridXY[i][j];
		}
		cout<<endl;
	}
	cout<<"\n\n";

	std::cout << "X-Z Axis.\n";
	for(int i =0; i < x; i++){
		for(int j=15; j<z; j++){
			if(gridXZ[i][j] == ""){
				cout<<".";
			}
			else cout<<gridXZ[i][j];
		}
		cout<<endl;
	}
	cout<<"\n\n";

	std::cout << "Y-Z Axis.\n";
	for(int i =0; i < y; i++){
		for(int j=15; j<z; j++){
			if(gridYZ[i][j] == ""){
				cout<<".";
			}
			else cout<<gridYZ[i][j];
		}
		cout<<endl;
	}
	cout<<"\n\n";
}
Display::~Display(){} //destructor

