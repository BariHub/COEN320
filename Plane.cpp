#include "Plane.h"


std::vector<int> Plane::mPlanesInAirSpace;

void* start(void* arg)
{
	Plane& plane = *(Plane*) arg;
	plane.updateLocation();
	return NULL;
}

Plane::Plane(int iID, float iArrivalTime, float iArrivalPosX, float iArrivalPosY, float iArrivalPosZ, float iArrivalVelX, float iArrivalVelY, float iArrivalVelZ):
	mID(iID), mTime(iArrivalTime)
{
	recvID = -1;
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

	if (pthread_create(&thread_id, NULL, start, (void*)this) != 0)
	{
		std::cerr << "Error in creating thread." << std::endl;
	}
}

Plane::~Plane()
{
}

void Plane::printArrivalInfo() const
{
	std::cout << std::left << std::setw(5) << "ID"
						<< std::setw(10) << "ToA" 
						<< std::setw(10) << "Init Pos X" 
						<< std::setw(10) << "Init Pos Y" 
						<< std::setw(10) << "Init Pos Z" 
						<< std::setw(10) << "Init Vel X"
						<< std::setw(10) << "Init Vel Y" 
						<< std::setw(10) << "Init Vel Z" 
						<< std::endl;

	std::cout << std::left << std::setw(5) << mID
						<< std::setw(10) << mTime 
						<< std::setw(10) << mInitPosition[0] 
						<< std::setw(10) << mInitPosition[1] 
						<< std::setw(10) << mInitPosition[2] 
						<< std::setw(10) << mInitVelocity[0] 
						<< std::setw(10) << mInitVelocity[1] 
						<< std::setw(10) << mInitVelocity[2] 
						<< std::endl;
}

void Plane::printInfo() const
{
	std::cout << std::left << std::setw(5) << mID
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

const std::vector<float> Plane::getPosition() const
{
	return this->mPosition;
}

const std::vector<float> Plane::getVelocity() const
{
	return this->mVelocity;
}

int Plane::updateLocation()
{
	plane_info info; // info of plane to be sent over
	MsgToPlane pMsg; // msg to be received by plane object
	std::string id = std::to_string(mID);
	const char* plane_ID = id.c_str();
	cTimer timer(1, 0); // update every 1 second

	name_attach_t *attach;
	attach = name_attach(NULL, plane_ID, 0);
	if(attach == NULL)
	{
		//printf("Could not attach plane ID: %d to channel", mID);
		return EXIT_FAILURE;
	}

	while (mPosition[0] <= 100000 && mPosition[0] >= 0 && mPosition[1] <= 100000 && mPosition[1] >= 0 &&
			mPosition[2] >= 15000 && mPosition[2] <= 40000) // if within airspace
	{
		if(find(mPlanesInAirSpace.begin(), mPlanesInAirSpace.end(), this->mID) == mPlanesInAirSpace.end())
		{
			mPlanesInAirSpace.push_back(this->mID);
		}

		this->mPosition[0] += mVelocity[0];
		this->mPosition[1] += mVelocity[1];
		this->mPosition[2] += mVelocity[2];
		//printf("Plane: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]);
		info = {mID, mPosition[0], mPosition[1], mPosition[2], mVelocity[0], mVelocity[1], mVelocity[2]};
		recvID = MsgReceive(attach->chid, &pMsg, sizeof(pMsg), NULL); // see if anything received

		if(pMsg.header.type == 0x00) // send info, if it wants it
		{
			MsgReply(recvID, EOK, &info, sizeof(info));
		}
		else if(pMsg.header.type == 0x02) // it is giving the plane info
		{
			printf("Commands received: speed change (%f, %f, %f)", pMsg.speedx, pMsg.speedy, pMsg.speedz);
			setVelocity(pMsg.speedx, pMsg.speedy, pMsg.speedz);
		}

		timer.waitTimer(); // after updating, wait until timer is ready for next 1 second interval
	}
	mPlanesInAirSpace.erase(std::remove(mPlanesInAirSpace.begin(), mPlanesInAirSpace.end(), this->mID), mPlanesInAirSpace.end());
	name_detach(attach, 0);
	pthread_exit(NULL);
	return 0;
}

const int Plane::getAirSpaceSize() const
{
	return mPlanesInAirSpace.size();
}

float distanceBetweenPlanes(const Plane& p1, const Plane& p2) { //distance between two planes function to send to computer sys
	float x = p1.mPosition[0] - p2.mPosition[0];
	float y = p1.mPosition[1] - p2.mPosition[1];
	float z = p1.mPosition[2] - p2.mPosition[2];
	return sqrt(x * x + y * y + z * z);
}
