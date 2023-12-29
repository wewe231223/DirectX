#pragma once
class Application{
public:
	Application(const Application& other) = delete;
	Application() = default;
	Application(HINSTANCE hInstance,LPCWSTR wcpWindowName);
	virtual ~Application() {};
private:
	HWND m_hWnd{};
	HINSTANCE m_hInstance{};
	HACCEL m_hHaccelTable{};
	MSG m_mMsg{};
public:
	static LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual void Loop();
	virtual void Update(float fDeltaTime) {};
	virtual void Render() {};
};

