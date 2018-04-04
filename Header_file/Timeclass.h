#ifndef _TIMECLASS_H_
#define _TIMECLASS_H_

#include <Windows.h>

class TimerClass{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency; 
	float m_ticksPerMs; 
	INT64 m_startTime; 
	float m_frameTime;


};

#endif