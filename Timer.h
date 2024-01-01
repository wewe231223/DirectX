#pragma once
class Timer{
public:
	Timer();
private:
	float m_fSecondsPerCount{};
	float m_fDeltaTime{ -1.f };

	INT64 m_nBaseTime{};
	INT64 m_nPausedTime{};
	INT64 m_nStopedTime{};
	INT64 m_nPrevTime{};
	INT64 m_nCurrentTime{};

	bool m_bStopped{false};

	float m_fFpsTimeElapsed{ 0.f };
	int m_nFrameCount{ 0 };

	std::wstring m_wsWindowTextBuffer{L"FPS : "};
public:
	float TimeElapsed() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Update();

	std::wstring GetFpsString();

};

