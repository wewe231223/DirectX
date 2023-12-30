#pragma once
class Timer{
public:
	Timer();
private:
	float m_fSecondsPerCount{};
	float m_fDeltaTime{};

	INT64 m_nBaseTime{};
	INT64 m_nPausedTime{};
	INT64 m_nStopedTime{};
	INT64 m_nPrevTime{};
	INT64 m_nCurrentTime{};

	bool m_bStoped{false};
public:
	float TimeElapsed() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Update();


};

