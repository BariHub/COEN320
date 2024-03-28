#include "AirTrafficControl.h"
#include <iostream>

int main()
{
	AirTrafficControl atc;
	
	if (atc.importData() == -1)
	{
		std::cerr << "Exiting..." << std::endl;
			return -1;
	}

	atc.printInfo();


	return 0;
}