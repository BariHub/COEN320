#include "AirTrafficControl.h"
#include "Radar.h"
#include "Comms.h"
#include "Display.h"
#include "Comp_Sys.h"
#include "OperatorConsole.h"
#include "Comms.h"
#include <iostream>

int main()
{
	AirTrafficControl atc;
	Radar rad;
	Display displaySystem;
	CompSys computerSystem;
	CommSystem CommSys;
	OperatorConsole OP;

	atc.run();
	
	return 0;
}
