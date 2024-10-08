/* Author: Christian Tiriolo
 * Shown during COEN320 tutorials
 */


#include "cTimer.h"
//everything is already called inside the constructor so we have encapsulation
cTimer::cTimer(uint32_t sec, uint32_t msec) {
	channel_id = ChannelCreate(0);
	connection_id = ConnectAttach(0,0,channel_id,0,0);
	if(connection_id == -1){
		std::cerr << "Timer, Connect Attach error : " << errno << "\n";
	}
	
	SIGEV_PULSE_INIT(&sig_event, connection_id, SIGEV_PULSE_PRIO_INHERIT, 1, 0);
	//debugcout << "TIMER pulse initiated" << endl;
	//
	if (timer_create(CLOCK_REALTIME, &sig_event, &timer_id) == -1){
		std::cerr << "Timer, Init error : " << errno << "\n";
	}
	
	setTimerSpec(sec,1000000* msec); //our own fct to setup the tv_sec and tv_nsec and so on
			
	cycles_per_sec = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
	

}

cTimer::~cTimer() {
	// TODO Auto-generated destructor stub
}

void cTimer::startTimer(){
	timer_settime(timer_id, 0, &timer_spec, NULL);
}
void cTimer::setTimerSpec(uint32_t sec, uint32_t nano){ // pure periodic timer
	timer_spec.it_value.tv_sec = sec;
	timer_spec.it_value.tv_nsec = nano;
	timer_spec.it_interval.tv_sec = sec;
	timer_spec.it_interval.tv_nsec = nano;
	timer_settime(timer_id, 0, &timer_spec, NULL); //starts the timer
}
void cTimer::waitTimer(){
	int rcvid;
	rcvid = MsgReceive(channel_id, &msg_buffer, sizeof(msg_buffer), NULL);
}
void cTimer::tick(){ // measures time
	tick_cycles = ClockCycles();
}
double cTimer::tock(){ // obtain elapsed time from last time u called tick
	tock_cycles = ClockCycles();
	return (double)((int)(((double)(tock_cycles-tick_cycles)/cycles_per_sec)*100000))/10;
}

