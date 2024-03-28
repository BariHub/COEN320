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

	int importData();
	void printInfo() const;

	std::vector<Plane*> mPlanes;
};

