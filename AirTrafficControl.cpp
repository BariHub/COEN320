#include "AirTrafficControl.h"

AirTrafficControl::AirTrafficControl()
{
}

AirTrafficControl::~AirTrafficControl()
{
}

int AirTrafficControl::generateData(int iNumPlanes)
{
	int fd;
	int size_written;
	fd = creat("/data/home/qnxuser/planes.txt", S_IWUSR | S_IRUSR | S_IXUSR);

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
		std::string line = std::to_string(wID) + " " + std::to_string(0) + " " + std::to_string(wPos[0]) + " " + std::to_string(wPos[1])
				+ " " + std::to_string(wPos[2]) + " " + std::to_string(wVel[0])
				+ " " + std::to_string(wVel[1]) + " " + std::to_string(wVel[2]) + "\n";
		std::cout << line.c_str() << std::endl;

		size_written = write(fd, line.c_str(), sizeof(line.size()));
		if( size_written != sizeof( line.size() ) ) {
	        perror( "Error writing myfile.dat" );
	        return EXIT_FAILURE;
	    }
		/*Plane* wPlane = new Plane(wID, 0, wPos[0], wPos[1], wPos[2], wVel[0], wVel[1], wVel[2]);
		mPlanes.push_back(wPlane);*/
		wID++;
		wSide++;
		wSide = wSide % 4;
	}
	close(fd);
	/*for(Plane *wPlane : mPlanes)
	{
		if(pthread_join(wPlane->thread_id, NULL) != 0)
		{
			std::cerr << "pthread_join error" << std::endl;
			exit(1);
		}
	}*/

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

int AirTrafficControl::exportData()
{

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
