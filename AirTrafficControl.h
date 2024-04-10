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
	AirTrafficControl();
	~AirTrafficControl();

	int generateData(int, std::string); // generate the plane files, low, med, high
	int importData(std::string);
	void run();
	void printInfo() const;

	std::vector<Plane*> mPlanes;
};

