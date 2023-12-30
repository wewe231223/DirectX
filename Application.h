#pragma once
class Application{
public:
	Application(const Application& other) = delete;
	Application() = default;
	Application(HINSTANCE hInstance,LPCWSTR wcpWindowName);
	Application(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW* pWindowProperties);
	virtual ~Application() {};
protected:
	HWND m_hWnd{};
	HINSTANCE m_hInstance{};
	HACCEL m_hHaccelTable{};
	MSG m_mMsg{};
public:
	LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual void Loop();
	virtual void Update(float fDeltaTime) {};
	virtual void Render() {};
public:
	HINSTANCE GetHandleInstance();
	HWND GetWindowHandle();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ApplicationFunctions {
	bool SetMainApplication(Application* app);
	LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DirectXApplication :public Application {
public:
	DirectXApplication() = default;
	DirectXApplication(const DirectXApplication& other) = delete;
	DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName) : Application(hInstance, wcpWindowName) {};
	DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW pWindowProperties);
	virtual ~DirectXApplication();
public:
	LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual void Loop() override;
	virtual void Update(float fDeltaTime) override;
	virtual void Render() override;
private:
	bool m_bPaused{ false };
	bool m_bMinimized{ false };
	bool m_bMaximized{ false };
	bool m_bResizing{ false };
	bool m_b4xMsaaState{ false };

	UINT n4xMsaaQuality{ 0 };



};