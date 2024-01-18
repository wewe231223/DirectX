#pragma once
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	bool m_bPaused{ false };
	bool m_bMinimized{ false };
	bool m_bMaximized{ false };
	bool m_bResizing{ false };
	bool m_b4xMsaaState{ false };

	int m_nClientWidth{ FRAMEBUFFER_WIDTH };
	int m_nClientHeight{ FRAMEBUFFER_HEIGHT };

public:
	virtual LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void Loop();
	virtual void Update(float fDeltaTime) {};
	virtual void Render() {};
public:
	HINSTANCE GetHandleInstance() const { return m_hInstance; };
	HWND GetWindowHandle() const { return m_hWnd; };
	ApplicationFunctions::WindowInfo GetWindowInfo() const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ApplicationFunctions {
	bool SetMainApplication(Application* app);
	WindowInfo GetMainApplicationInfo();
	LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK DefaultProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Timer;
class Scene;

class DirectXApplication :public Application {
public:
	DirectXApplication() = default;
	DirectXApplication(const DirectXApplication& other) = delete;
	DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName);
	DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW pWindowProperties);
	virtual ~DirectXApplication();
public:
	virtual LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	virtual void Loop() override;
	virtual void Update(float fDeltaTime) override;
	virtual void Render() override;
private:
	std::unique_ptr<Timer> m_timer{ nullptr };

	ComPtr<IDXGIFactory4> m_dxgiFactory{};
	ComPtr<IDXGISwapChain> m_dxgiSwapChain{};
	ComPtr<ID3D12Device> m_d3dDevice{};

	UINT m_n4xMsaaQuality{ 0 };

	ComPtr<ID3D12Fence> m_d3dFence{};
	UINT64 m_nCurrentFence{ 0 };

	ComPtr<ID3D12CommandQueue> m_d3dCommandQueue{};
	ComPtr<ID3D12CommandAllocator> m_d3dCommandAllocator{};
	ComPtr<ID3D12GraphicsCommandList> m_d3dCommandList{};

	int m_nCurrentBackBuffer{ 0 };
	ComPtr<ID3D12Resource> m_d3dSwapChainBuffer[SWAPCHAINBUFFERCOUNT]{};
	ComPtr<ID3D12Resource> m_d3dDepthStencilBuffer{};
	ComPtr<ID3D12DescriptorHeap> m_d3dRenderTargetViewHeap{};
	ComPtr<ID3D12DescriptorHeap> m_d3dDepthStencilViewHeap{};

	D3D12_VIEWPORT m_d3dScreenViewPort{};
	D3D12_RECT m_d3dSissorRect{};

	UINT m_nRenderTargetViewDescriptorSize{ 0 };
	UINT m_nDepthStencilViewDescriptorSize{ 0 };
	UINT m_nResourceBufferDecriptorSize{ 0 };

	D3D_DRIVER_TYPE m_d3dDriverType{ D3D_DRIVER_TYPE_HARDWARE };
	DXGI_FORMAT m_dxgiBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT m_dxgiDepthStencilFormat{ DXGI_FORMAT_D24_UNORM_S8_UINT };

	D3D_FEATURE_LEVEL m_d3dDirectXFeatureLevel{ D3D_FEATURE_LEVEL_11_0 };

	std::unique_ptr<Scene> m_pScene{ nullptr };
private:
	bool Initialize();
	bool InitializeDirect3D();
	void CreateCommandList();
	void CreateSwapChain();
	void CreateRenderTargetViewDescriptorHeap();
	void CreateDepthStencilViewDescriptorHeap();
	bool Resize();
	void FlushCommandQueue();


	void LogAdapters();

	ID3D12Resource* GetCurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;


};