#include "Plane.h"


std::vector<int> Plane::mPlanesInAirSpace;

void* start(void* arg) // function to be passed into pthread_create to start updating its position concurrently with other planes
{
	Plane& plane = *(Plane*) arg; // args for the update location, the plane itself
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

	if (pthread_create(&thread_id, NULL, start, (void*)this) != 0) // create thread, passing the plane itself as arg
	{
		std::cerr << "Error in creating thread." << std::endl;
	}
}

Plane::~Plane()
{
}

void Plane::printArrivalInfo() const // print initial arrival info, not used
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

void Plane::printInfo() const // print information about each plane
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
void Plane::setVelocityX(float x){
	this->mVelocity[0] = x;
}
void Plane::setVelocityY(float y){
	this->mVelocity[1] = y;
}
void Plane::setVelocityZ(float z){
	this->mVelocity[2] = z;
}
const std::vector<float> Plane::getVelocity() const
{
	return this->mVelocity;
}

int Plane::updateLocation() // important function, updates each plane objects position based on velocity
{
	plane_info info; // info of plane to be sent over
	MsgToPlane pMsg; // msg to be received by plane object
	std::string id = std::to_string(mID);
	const char* plane_ID = id.c_str();
	cTimer timer(1, 0); // update every 1 second

	name_attach_t *attach;
	attach = name_attach(NULL, plane_ID, 0); // create a channel server for each plane
	if(attach == NULL)
	{
		printf("Could not attach plane ID: %d to channel", mID);
		return EXIT_FAILURE;
	}

	while (mPosition[0] <= 100000 && mPosition[0] >= 0 && mPosition[1] <= 100000 && mPosition[1] >= 0 &&
			mPosition[2] >= 15000 && mPosition[2] <= 40000) // if within airspace
	{
		if(find(mPlanesInAirSpace.begin(), mPlanesInAirSpace.end(), this->mID) == mPlanesInAirSpace.end()) // if not in air space, add ID in
		{
			mPlanesInAirSpace.push_back(this->mID);
		}

		this->mPosition[0] += mVelocity[0]; // update velocity, time is 1s, therefore velocity[0]*1
		this->mPosition[1] += mVelocity[1];
		this->mPosition[2] += mVelocity[2];
		//printf("Plane: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]);
		info = {mID, mPosition[0], mPosition[1], mPosition[2], mVelocity[0], mVelocity[1], mVelocity[2]}; // packing plane information into a struct to be sent in case
		// something requests it, for example the radar
		recvID = MsgReceive(attach->chid, &pMsg, sizeof(pMsg), NULL); // see if plane received any messages

		if(pMsg.header.type == 0x00) // send info, if it wants it
		{
			MsgReply(recvID, EOK, &info, sizeof(info)); // reply back if received
		}
		else if(pMsg.header.type == 0x02) // receiving plane info from elsewhere, from console through communications
		{
			//printf("Plane: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]);
			printf("Commands received: speed change (%f, %f, %f)", pMsg.speedx, pMsg.speedy, pMsg.speedz);
			if(pMsg.speedx != -1.0){
				setVelocityX(pMsg.speedx);
				printf("PlaneX: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]); // change speed based on message received
			}
			else if(pMsg.speedy != -1.0){
				setVelocityY(pMsg.speedy);
				printf("PlaneY: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]);
			}
			else if(pMsg.speedz != -1.0){
				setVelocityZ(pMsg.speedz);
				printf("PlaneZ: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]);
			}
		}
		MsgReply(recvID, EOK, 0, 0); // reply back that we received

		//printf("Plane: %d %f %f %f \n", mID, mVelocity[0], mVelocity[1], mVelocity[2]);
		timer.waitTimer(); // after updating, wait until timer is ready for next interval, ensure it updates every 1 second
	}
	mPlanesInAirSpace.erase(std::remove(mPlanesInAirSpace.begin(), mPlanesInAirSpace.end(), this->mID), mPlanesInAirSpace.end()); // once out of while
	// loop, it means plane is out of airspace, therefore remove it
	name_detach(attach, 0); // destroy channel
	pthread_exit(NULL); // destroy thread and clean exit
	return 0;
}

const int Plane::getAirSpaceSize() const // get air space
{
	return mPlanesInAirSpace.size();
}

float distanceBetweenPlanes(const Plane& p1, const Plane& p2) { // most likely unused, we have similar function elsewhere
	float x = p1.mPosition[0] - p2.mPosition[0];
	float y = p1.mPosition[1] - p2.mPosition[1];
	float z = p1.mPosition[2] - p2.mPosition[2];
	return sqrt(x * x + y * y + z * z);
}
