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
	AirTrafficControl atc; // initialization of low, med, high files for congestion into qnxuser
	std::cout << "What is the desired congestion level: "; // ask for input on congestion level
	std::cin >> atc.level;
	std::cout << std::endl;

	Radar rad; // instantiation
	Display displaySystem; // instantiation
	CompSys computerSystem; // instantiation
	CommSystem CommSys; // instantiation
	OperatorConsole OP; // instantiation

	atc.run(); // running main program, threads are created for planes based on congestion level chosen
	

	atc.printInfo(); // prints final info of each plane. Its last position recorded upon exiting the air space

	printf("End of script.");
	return 0;
}
