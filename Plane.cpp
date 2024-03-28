#include "Plane.h"


std::vector<int> Plane::mPlanesInAirSpace;

//void* plane_routine(void* arg)
//{
//	Plane& plane = *(plane*) arg;
//	plane.updateLocation();
//	return NULL;
//}

Plane::Plane(int iID, float iArrivalTime, float iArrivalPosX, float iArrivalPosY, float iArrivalPosZ, float iArrivalVelX, float iArrivalVelY, float iArrivalVelZ):
	mID(iID), mTime(iArrivalTime)
{
	mInitPosition.push_back(iArrivalPosX);
	mInitPosition.push_back(iArrivalPosY);
	mInitPosition.push_back(iArrivalPosZ);
	mInitVelocity.push_back(iArrivalVelX);
	mInitVelocity.push_back(iArrivalVelY);
	mInitVelocity.push_back(iArrivalVelZ);

	mPosition.push_back(iArrivalPosX);
	mPosition.push_back(iArrivalPosY);
	mPosition.push_back(iArrivalPosZ);
	mVelocity.push_back(iArrivalVelX);
	mVelocity.push_back(iArrivalVelY);
	mVelocity.push_back(iArrivalVelZ);

	/*if (pthread_create(&thread_id, NULL, fnc, (void*)this) != EOK)
	{
		
	}*/

}

Plane::~Plane()
{
}

void Plane::printArrivalInfo() const
{
	std::cout << std::left << std::setw(10) << "ID" 
						<< std::setw(10) << "ToA" 
						<< std::setw(10) << "Init Pos X" 
						<< std::setw(10) << "Init Pos Y" 
						<< std::setw(10) << "Init Pos Z" 
						<< std::setw(10) << "Init Vel X"
						<< std::setw(10) << "Init Vel Y" 
						<< std::setw(10) << "Init Vel Z" 
						<< std::endl;

	std::cout << std::left << std::setw(10) << mID 
						<< std::setw(10) << mTime 
						<< std::setw(10) << mInitPosition[0] 
						<< std::setw(10) << mInitPosition[1] 
						<< std::setw(10) << mInitPosition[2] 
						<< std::setw(10) << mInitVelocity[0] 
						<< std::setw(10) << mInitVelocity[1] 
						<< std::setw(10) << mInitVelocity[2] 
						<< std::endl;
}

void Plane::printCurrentInfo() const
{
	std::cout << std::left << std::setw(10) << mID 
						<< std::setw(10) << mPosition[0] 
						<< std::setw(10) << mPosition[1] 
						<< std::setw(10) << mPosition[2] 
						<< std::setw(10) << mVelocity[0] 
						<< std::setw(10) << mVelocity[1] 
						<< std::setw(10) << mVelocity[2] 
						<< std::endl;
}

void Plane::setPosition(float x, float y , float z)
{
	this->mPosition[0] = x;
	this->mPosition[1] = y;
	this->mPosition[2] = z;
}

void Plane::setVelocity(float x, float y, float z)
{
	this->mVelocity[0] = x;
	this->mVelocity[1] = y;
	this->mVelocity[2] = z;
}

int Plane::updateLocation()
{
	cTimer timer(1, 0, 1, 0);

	while (mPosition[0] < 100000 && mPosition[1] < 100000 && mPosition[2] < 25000 && mPosition[2] > 15000)
	{
		mPlanesInAirSpace.push_back(mID);

		this->mPosition[0] += mVelocity[0];
		this->mPosition[1] += mVelocity[1];
		this->mPosition[2] += mVelocity[2];



	}
	return 0;
}

float distanceBetweenPlanes(const Plane& p1, const Plane& p2) { //distance between two planes function to send to computer sys
	float x = p1.mPosition[0] - p2.mPosition[0];
	float y = p1.mPosition[1] - p2.mPosition[1];
	float z = p1.mPosition[2] - p2.mPosition[2];
	return sqrt(x * x + y * y + z * z);
}
