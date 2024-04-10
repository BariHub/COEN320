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
	std::string type;
	while(true)
	{
		std::cout << "What is the desired congestion level: ";
		std::cin >> type;
		std::cout << std::endl;
		if(type == "low" || type == "med" || type == "high")
		{
			atc.level = type;
			break;
		}
		std::cout << "Incorrect congestion level, try again.\n";
	}

	Radar rad;
	Display displaySystem;
	CompSys computerSystem;
	CommSystem CommSys;
	OperatorConsole OP;

	atc.run();
	
	return 0;
}
