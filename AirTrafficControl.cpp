#include "AirTrafficControl.h"

AirTrafficControl::AirTrafficControl() // generates all 3 files, choose which one later
{
	generateData(10, "low"); // number of planes is 10 for low congestion
	generateData(20, "med"); // number of planes is 20 for med congestion
	generateData(30, "high"); // number of planes is 30 for high congestion
}

AirTrafficControl::~AirTrafficControl()
{
	for (Plane* planePtr : mPlanes) // free up all plane pointers and clear vector
	{
		delete planePtr;
	}
	mPlanes.clear();
}

int AirTrafficControl::run() // here, based on what the user choosees, set the congestion level of airspace and import info and run
{
	if(level == "low")
	{
		importData("low"); // import low congestion file that was created in constructor and run the threads
	}
	else if (level == "med")
	{
		importData("med");
	}
	else if (level == "high")
	{
		importData("high");
	}
	return 0;
}

int AirTrafficControl::generateData(int iNumPlanes, std::string type) // generate the data for x number of planes
{
	int fd;
	int size_written;
	std::string path = "/data/home/qnxuser/"+ type +".txt";
	fd = creat(path.c_str(), S_IWUSR | S_IRUSR | S_IXUSR); // create/read file in qnxuser directory

	// the way generated the planes location and velocity is based on which side we want to spawn them in.
	// the airspace has four sides, hence the variable below, wSide. we go through each side incrementally and generate a plane one at a time.
	// a plane is generated randomly, just barely on the side of the boundary box. we are assuming they all come in at the same time and on the edge
	// of each side of the boundary box
	// we are assuming the planes are facing perpendicular to each face of the boundary box
	int wSide = 0;
	int wID = 0; // plane ID, each plane gets a unique ID, and it is done incrementally
	std::vector<float> wPos(3, 0); // vector that will hold position of plane
	std::vector<float> wVel(3, 0); // vector that will hold velocity of plane
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // random number generator, based on computer time, so we get values generated randomly every
	// time
	for (int i = 0; i < iNumPlanes; i++)
	{
		switch (wSide)
		{
		case 0:
			wPos[0] = 100000; // based on the boundary box, the plane is being generated on the position x side of the box
			wPos[1] = (std::rand() % 100000 + 1); // randomly generated in the y axis, facing the x plane
			wPos[2] = (std::rand() % 25000 + 1) + 15000; // randomly generated in the z axis, facing the x plane
			wVel[0] = -1000; // going inward into the box, which happens to be towards the negative x axis
			wVel[1] = 0;
			wVel[2] = 0;
			break;
		case 1:
			wPos[0] = (std::rand() % 100000 + 1); // randomly generated in the x axis, facing the y plane
			wPos[1] = 100000; // based on the boundary box, the plane is being generated on the position y side of the box
			wPos[2] = (std::rand() % 25000 + 1) + 15000; // randomly generated in the z axis, facing the x plane
			wVel[0] = 0;
			wVel[1] = -1000; // going inward into the box, which happens to be towards the negative y axis
			wVel[2] = 0;
			break;
		case 2:
			wPos[0] = 0;
			wPos[1] = (std::rand() % 100000 + 1); // similar to above
			wPos[2] = (std::rand() % 25000 + 1) + 15000;
			wVel[0] = 1000; // going inward into the box, which happens to be towards the positive x axis
			wVel[1] = 0;
			wVel[2] = 0;
			break;
		case 3:
			wPos[0] = (std::rand() % 100000 + 1); // similar to above
			wPos[1] = 0;
			wPos[2] = (std::rand() % 25000 + 1) + 15000;
			wVel[0] = 0;
			wVel[1] = 1000; // going inward into the box, which happens to be towards the positive y axis
			wVel[2] = 0;
			break;
		}
		std::string line = std::to_string(wID) + " " + std::to_string(0) + " " + std::to_string(wPos[0]) + " " + std::to_string(wPos[1])
				+ " " + std::to_string(wPos[2]) + " " + std::to_string(wVel[0])
				+ " " + std::to_string(wVel[1]) + " " + std::to_string(wVel[2]) + "\n"; // time is set to 0, we assume they all come in at the same time

		size_written = write(fd, line.c_str(), line.size()); // write to file, the info of plane, id, time, pos3, vel3
		if( size_written != line.size() ) {
	        perror( "Error writing myfile.dat" );
	        return EXIT_FAILURE;
	    }
		/*Plane* wPlane = new Plane(wID, 0, wPos[0], wPos[1], wPos[2], wVel[0], wVel[1], wVel[2]);
		mPlanes.push_back(wPlane);*/
		wID++; // increment ID by 1
		wSide++; // increment side by 1, so we generate one plane on front side first, then right side, then back sie, the left side then back to front side again and
		// continue the trend
		wSide = wSide % 4; // rectangle only has 4 sides
	}
	close(fd); // close the file
	return 0;
}

int AirTrafficControl::importData(std::string type)
{
	std::ifstream wFin;
	wFin.open("/data/home/qnxuser/" + type + ".txt", std::ios::in); // import data located in qnxuser

	int wID = 0;
	float wArrivalTime = 0.0;
	float arrivalPosX, arrivalPosY, arrivalPosZ, arrivalVelX, arrivalVelY, arrivalVelZ = 0.0; // variables for information in file

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
		wFin >> wID >> wArrivalTime >> arrivalPosX >> arrivalPosY >> arrivalPosZ >> arrivalVelX >> arrivalVelY >> arrivalVelZ; // get info from file
		Plane* wPlane = new Plane(wID, wArrivalTime, arrivalPosX, arrivalPosY, arrivalPosZ, arrivalVelX, arrivalVelY, arrivalVelZ); // create plane class dynamically
		mPlanes.push_back(wPlane); // add plane pointer to vector to keep track of all planes created
		if (wPlane == nullptr)
		{
			std::cerr << "Failed to create plane object" << std::endl;
			return -1;
		}
	}
	wFin.close();

	for(Plane *wPlane : mPlanes)
	{
		if(pthread_join(wPlane->thread_id, NULL) != 0) // here we have main thread join with its child threads, because we dont want the main thread to finish
		{ // before its children. the main will wait for all its plane threads to finish then, resume.
			std::cerr << "pthread_join error" << std::endl;
			exit(1);
		}
	}
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

	for (const Plane* wPlane : mPlanes) // print plane info
	{
		wPlane->printInfo();
	}
}
