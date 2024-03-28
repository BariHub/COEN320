#pragma once
//#include <pthread.h>
#include <vector>
#include <iomanip>
#include <iostream>
#include "cTimer.h"

class Plane
{
public:
	Plane(int, float, float, float, float, float, float, float);
	~Plane();

	void printArrivalInfo() const;
	void printCurrentInfo() const;
	void setPosition(float, float, float);
	void setVelocity(float, float, float);

	int updateLocation();

	int mID;
	float mTime;
	//pthread_t thread_id;
	static std::vector<int> mPlanesInAirSpace;
	
	std::vector<float> mInitPosition; // x, y, z
	std::vector<float> mInitVelocity; // x, y, z
	std::vector<float> mPosition; // x, y, z
	std::vector<float> mVelocity; // x, y, z
};

