#include "pch.h"
#include "Timer.h"

Timer::Timer(){
	INT64 CountsPerSecond{};
	::QueryPerformanceFrequency((LARGE_INTEGER*)&CountsPerSecond);
	m_fSecondsPerCount = 1.f / static_cast<float>(CountsPerSecond);
}

float Timer::TimeElapsed() const{
	if (m_bStoped) {
		// 멈춰있는 경우 지나간 시간은 제외한다 
		return static_cast<float>(((m_nStopedTime - m_nPausedTime) - m_nBaseTime)) * m_fSecondsPerCount;
	}
	else {
		// 그렇지 않다면, 현재 시간을 기준으로 계산 
		return static_cast<float>((m_nCurrentTime - m_nPausedTime) - m_nBaseTime) * m_fSecondsPerCount;
	}
}

float Timer::DeltaTime() const{
	return m_fDeltaTime;
}

void Timer::Reset(){
	INT64 CurrentTime{};
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

	m_nBaseTime = CurrentTime;
	m_nPrevTime = CurrentTime;
	m_nStopedTime = 0;
	m_bStoped = false;
}

void Timer::Start(){
	INT64 StartTime{};
	::QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	if (m_bStoped) {
		m_nPausedTime += (StartTime - m_nStopedTime);
		m_nPrevTime = StartTime;
		m_bStoped = false;
	}
}

void Timer::Stop(){
	if (!m_bStoped) {
		INT64 CurrentTime{};
		::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
		m_nStopedTime = CurrentTime;
		m_bStoped = true;
	}
}

void Timer::Update(){
	if (m_bStoped) {
		m_fDeltaTime = 0.f;
		return;
	}

	INT64 CurrentTime{};
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	m_nCurrentTime = CurrentTime;
	m_fDeltaTime = static_cast<float>(CurrentTime - m_nPrevTime) * m_fSecondsPerCount;
	m_nPrevTime = CurrentTime;

	if (m_fDeltaTime < 0.f) {
		m_fDeltaTime = 0.f;
	}
}
