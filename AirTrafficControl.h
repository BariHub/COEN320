#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include "Plane.h"

class AirTrafficControl
{
public:
	AirTrafficControl(); // initializes all 3 congestion level files, low, med, high
	~AirTrafficControl();

	int generateData(int, std::string); // generate the plane files, low, med, high. GENERATES THE TEXT FILES, SO NO NEED TO ADD THEM SEPERATELY AS IT IS DONE HERE
	// AUTOMATICALLY WHEN RUNNING FOR THE FIRST TIME
	int importData(std::string); // import plane information from files to start the program
	int run(); // run file with the desired congestion level
	void printInfo() const; // print info of each plane

	std::vector<Plane*> mPlanes; // vector to hold all planes created
	std::string level; // level of congestion
};

