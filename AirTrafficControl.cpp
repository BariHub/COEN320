#include "AirTrafficControl.h"

AirTrafficControl::AirTrafficControl()
{
}

AirTrafficControl::~AirTrafficControl()
{
}

int AirTrafficControl::generateData(int iNumPlanes)
{
	std::ofstream wFout;
	wFout.open("planes.txt", std::ios::out);

	if (!wFout)
	{
		std::cerr << "Unable to open file." << std::endl;
		return -1;
	}

	int wSide = 0;
	// plane information, ID and its 3d pos and 3d vel
	int wID = 0;
	std::vector<float> wPos(3, 0);
	std::vector<float> wVel(3, 0);
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	for (int i = 0; i < iNumPlanes; i++)
	{
		switch (wSide)
		{
		case 0:
			wPos[0] = 100000;
			wPos[1] = (std::rand() % 100000 + 1);
			wPos[2] = (std::rand() % 25000 + 1) + 15000;
			wVel[0] = -500;
			wVel[1] = 0;
			wVel[2] = 0;
			break;
		case 1:
			wPos[0] = (std::rand() % 100000 + 1);
			wPos[1] = 100000;
			wPos[2] = (std::rand() % 25000 + 1) + 15000;
			wVel[0] = 0;
			wVel[1] = -500;
			wVel[2] = 0;
			break;
		case 2:
			wPos[0] = 0;
			wPos[1] = (std::rand() % 100000 + 1);
			wPos[2] = (std::rand() % 25000 + 1) + 15000;
			wVel[0] = 500;
			wVel[1] = 0;
			wVel[2] = 0;
			break;
		case 3:
			wPos[0] = (std::rand() % 100000 + 1);
			wPos[1] = 0;
			wPos[2] = (std::rand() % 25000 + 1) + 15000;
			wVel[0] = 0;
			wVel[1] = 500;
			wVel[2] = 0;
			break;
		}
		Plane* wPlane = new Plane(wID, 0, wPos[0], wPos[1], wPos[2], wVel[0], wVel[1], wVel[2]);
		mPlanes.push_back(wPlane);
		wID++;
		wSide++;
		wSide = wSide % 4;
	}
	for(Plane *wPlane : mPlanes)
	{
		if(pthread_join(wPlane->thread_id, NULL) != 0)
		{
			std::cerr << "pthread_join error" << std::endl;
			exit(1);
		}
	}

	return 0;
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
						<< std::setw(10) << "Pos X"
						<< std::setw(10) << "Pos Y"
						<< std::setw(10) << "Pos Z"
						<< std::setw(10) << "Vel X"
						<< std::setw(10) << "Vel Y"
						<< std::setw(10) << "Vel Z"
						<< std::endl;

	for (const Plane* wPlane : mPlanes)
	{
		wPlane->printInfo();
	}
}
