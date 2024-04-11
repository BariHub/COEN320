#pragma once
#include <pthread.h>
#include <vector>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <sys/dispatch.h>
#include "cTimer.h"
#include "Struct_definitions.h"

class Plane
{
public:
	Plane(int, float, float, float, float, float, float, float); // initialization of plane, create thread in here
	~Plane();

	void printArrivalInfo() const; // print function of when the planes arrive in air space
	void printInfo() const; // prints current info, such as id, pos and vel
	void setPosition(float, float, float); // sets position
	void setVelocity(float, float, float); // sets velocity for all 3
	void setVelocityX(float);
	void setVelocityY(float);
	void setVelocityZ(float);
	const std::vector<float> getPosition() const; // get current position
	const std::vector<float> getVelocity() const; // get current velocity
	const int getAirSpaceSize() const; // get the number of planes in air space

	int updateLocation(); // update location of plane based on velocity of information received from console

	int mID; // identification of plane
	int recvID; // receive id
	float mTime;
	pthread_t thread_id; // thread id
	static std::vector<int> mPlanesInAirSpace; // static var of planes in air space
	
	std::vector<float> mInitPosition; // x, y, z, unnecessary
	std::vector<float> mInitVelocity; // x, y, z, unnecessary
	std::vector<float> mPosition; // x, y, z, actual coords that are being updated mPosition[0] is x, mPosition[1] is y, mPosition[2] is z
	std::vector<float> mVelocity; // x, y, z, actual values that are being updated
};

