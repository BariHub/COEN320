#include "AirTrafficControl.h"
#include <iostream>

int main()
{
	AirTrafficControl atc;
	atc.generateData(2);
	
	/*if (atc.importData() == -1)
	{
		std::cerr << "Exiting..." << std::endl;
		return -1;
	}*/

	atc.printInfo();

	printf("End of script.");
	return 0;
}
