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
	std::cout << "What is the desired congestion level: ";
	std::cin >> atc.level;
	std::cout << std::endl;

	Radar rad;
	Display displaySystem;
	CompSys computerSystem;
	CommSystem CommSys;
	OperatorConsole OP;

	cout<<atc.run()<<endl;
	

	atc.printInfo();

	printf("End of script.");
	return 0;
}
