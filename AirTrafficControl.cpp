#include "AirTrafficControl.h"

AirTrafficControl::AirTrafficControl()
{
}

AirTrafficControl::~AirTrafficControl()
{
}

int AirTrafficControl::importData()
{
	std::ifstream wFin;
	wFin.open("planes.txt", std::ios::in);

	int wID = 0;
	float wArrivalTime = 0.0;
	float arrivalPosX, arrivalPosY, arrivalPosZ, arrivalVelX, arrivalVelY, arrivalVelZ = 0.0;

	if (!wFin)
	{
		std::cerr << "Unable to open file." << std::endl;
		return -1;
	}

	if (wFin.peek() == wFin.eof())
	{
		std::cerr << "The file is empty." << std::endl;
		return -1;
	}

	while (!wFin.eof())
	{
		wFin >> wID >> wArrivalTime >> arrivalPosX >> arrivalPosY >> arrivalPosZ >> arrivalVelX >> arrivalVelY >> arrivalVelZ;
		Plane* wPlane = new Plane(wID, wArrivalTime, arrivalPosX, arrivalPosY, arrivalPosZ, arrivalVelX, arrivalVelY, arrivalVelZ);

		if (wPlane == nullptr)
		{
			std::cerr << "Failed to create plane object" << std::endl;
			return -1;
		}

		mPlanes.push_back(wPlane);
	}

	wFin.close();

	return 0;
}

void AirTrafficControl::printInfo() const
{
	std::cout << std::left << std::setw(5) << "ID" 
						<< std::setw(10) << "Curr Pos X" 
						<< std::setw(10) << "Curr Pos Y" 
						<< std::setw(10) << "Curr Pos Z" 
						<< std::setw(10) << "Curr Vel X" 
						<< std::setw(10) << "Curr Vel Y" 
						<< std::setw(10) << "Curr Vel Z" 
						<< std::endl;

	for (const Plane* wPlane : mPlanes)
	{
		wPlane->printCurrentInfo();
	}
}
