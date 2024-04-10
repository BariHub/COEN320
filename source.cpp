#include "AirTrafficControl.h"
#include "Radar.h"
#include "Comms.h"
#include "Display.h"
#include "Comp_Sys.h"
#include <iostream>

int main()
{
	Radar rad;
	Display displaySystem;
	CompSys computerSystem;
	AirTrafficControl atc;

	atc.run();
	



	/*if (atc.importData() == -1)
	{
		std::cerr << "Exiting..." << std::endl;
		return -1;
	}*/

	atc.printInfo();

	printf("End of script.");
	return 0;
}
