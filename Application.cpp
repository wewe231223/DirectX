#include "pch.h"
#include "Timer.h"
#include "Application.h"

static Application* mApp = nullptr;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application(HINSTANCE hInstance,LPCWSTR wcpWindowName){
	m_hInstance = hInstance;
    WCHAR ClassName[100]{};
    ::LoadStringW(m_hInstance, IDC_DIRECTX, ClassName, 100);
    WNDCLASSEXW wcex = ::GetDefaultWindowProperties(m_hInstance);
    wcex.lpszClassName = ClassName;
    wcex.lpfnWndProc = ApplicationFunctions::Procedure;
    ::RegisterClassExW(&wcex);
    m_hWnd = ::CreateWindowExW(0L, ClassName, wcpWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 
        nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) {
        exit(-1);
    }
    m_hHaccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_DIRECTX));
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
}

Application::Application(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW* pWindowProperties){
    m_hInstance = hInstance;
    WCHAR ClassName[100]{};
    ::LoadStringW(m_hInstance, IDC_DIRECTX, ClassName, 100);
    ::RegisterClassExW(pWindowProperties);
    m_hWnd = ::CreateWindowExW(0L, ClassName, wcpWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT,
        nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) {
        exit(-1);
    }
    m_hHaccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_DIRECTX));
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
}

LRESULT Application::Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void Application::Loop(){
    while (true) {
        if (PeekMessage(&m_mMsg, NULL, 0, 0, PM_REMOVE)) {
            if (m_mMsg.message == WM_QUIT) break;
            if (!::TranslateAccelerator(m_mMsg.hwnd, m_hHaccelTable, &m_mMsg)) {
                ::TranslateMessage(&m_mMsg);
                ::DispatchMessage(&m_mMsg);
            }
            else {
                Update(0.f);
                Render();
            }
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicationFunctions::SetMainApplication(Application* app){
    if (mApp) return false;
    mApp = app;
    return true;
}
LRESULT ApplicationFunctions::Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    return mApp->Procedure(hWnd,message,wParam,lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DirectXApplication::DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName): Application(hInstance, wcpWindowName) {
    m_timer = std::make_unique<Timer>();
    Initialize();
}

DirectXApplication::DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW pWindowProperties){
}

DirectXApplication::~DirectXApplication(){
}

LRESULT DirectXApplication::Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void DirectXApplication::Loop(){
//   ThrowIfFailed(E_FAIL);
    while (true) {
        if (PeekMessage(&m_mMsg, NULL, 0, 0, PM_REMOVE)) {
            if (m_mMsg.message == WM_QUIT) break;
            if (!::TranslateAccelerator(m_mMsg.hwnd, m_hHaccelTable, &m_mMsg)) {
                ::TranslateMessage(&m_mMsg);
                ::DispatchMessage(&m_mMsg);
            }
            else {
                Update(0.f);
                Render();
            }
        }
    }
}

void DirectXApplication::Update(float fDeltaTime){
}

void DirectXApplication::Render(){
}

bool DirectXApplication::Initialize(){
    if (!InitializeDirect3D()) {
        return false;
    }
    Resize();
    return true;
}

bool DirectXApplication::InitializeDirect3D(){
#if defined(DEBUG) || defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> DebugController{};
        ThrowIfFailed(::D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
        DebugController->EnableDebugLayer();
    }
#endif //!defined(DEBUG) || defined(_DEBUG)
   
    ThrowIfFailed(::CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));
    HRESULT HardwareResult = ::D3D12CreateDevice(nullptr, m_d3dDirectXFeatureLevel, IID_PPV_ARGS(m_d3dDevice.GetAddressOf()));

    //if (FAILED(HardwareResult)) {
    //    ComPtr<IDXGIAdapter> WarpAdapter{};
    //    ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapter)));
    //    ThrowIfFailed(::D3D12CreateDevice(WarpAdapter.Get(), m_d3dDirectXFeatureLevel, IID_PPV_ARGS(&m_d3dDevice)));
    //}

    ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_d3dFence.GetAddressOf())));

    m_nRenderTargetViewDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_nDepthStencilViewDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_nResourceBufferDecriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels{};
    QualityLevels.Format = m_dxgiBackBufferFormat;
    QualityLevels.SampleCount = 4;
    QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    QualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &QualityLevels, sizeof(QualityLevels)));
    m_n4xMsaaQuality = QualityLevels.NumQualityLevels;


#if defined(DEBUG) || defined(_DEBUG)
    LogAdapters();
#endif //!defined(DEBUG) || defined(_DEBUG)

    CreateCommandList();
    //CreateSwapChain();
    //CreateRenderTargetViewDescriptorHeap();
    //CreateDepthStencilViewDescriptorHeap();


    return true;
}

void DirectXApplication::CreateCommandList(){
    D3D12_COMMAND_QUEUE_DESC Desc{};
    Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(m_d3dCommandQueue.GetAddressOf())));
    ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_d3dCommandAllocator.GetAddressOf())));
    ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_d3dCommandAllocator.Get(), NULL, IID_PPV_ARGS(m_d3dCommandList.GetAddressOf())));
    m_d3dCommandList->Close();
}

void DirectXApplication::CreateSwapChain(){
    m_dxgiSwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC Desc;
    Desc.BufferDesc.Width = m_nClientWidth;
    Desc.BufferDesc.Height = m_nClientHeight;
    Desc.BufferDesc.RefreshRate.Numerator = 60;
    Desc.BufferDesc.RefreshRate.Denominator = 1;
    Desc.BufferDesc.Format = m_dxgiBackBufferFormat;
    Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    Desc.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
    Desc.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;
    Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    Desc.BufferCount = SWAPCHAINBUFFERCOUNT;
    Desc.OutputWindow = m_hWnd;
    Desc.Windowed = true;
    Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ThrowIfFailed(m_dxgiFactory->CreateSwapChain(m_d3dCommandQueue.Get(), &Desc, m_dxgiSwapChain.GetAddressOf()));
}

void DirectXApplication::CreateRenderTargetViewDescriptorHeap(){
    D3D12_DESCRIPTOR_HEAP_DESC RenderTargetViewDesc{};
    RenderTargetViewDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    RenderTargetViewDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    RenderTargetViewDesc.NodeMask = 0;
    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&RenderTargetViewDesc, IID_PPV_ARGS(m_d3dRenderTargetViewHeap.GetAddressOf())));
}

void DirectXApplication::CreateDepthStencilViewDescriptorHeap(){
    D3D12_DESCRIPTOR_HEAP_DESC DepthStencilViewDescriptorHeap{};
    DepthStencilViewDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    DepthStencilViewDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DepthStencilViewDescriptorHeap.NodeMask = 0;
    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&DepthStencilViewDescriptorHeap, IID_PPV_ARGS(m_d3dDepthStencilViewHeap.GetAddressOf())));
}


bool DirectXApplication::Resize(){
    return false;
}

void DirectXApplication::LogAdapters(){
    UINT i = 0;
    IDXGIAdapter* Adapter{ nullptr };
    std::vector<IDXGIAdapter*> AdapterList{};

    while (m_dxgiFactory->EnumAdapters(i, &Adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC Desc{};
        Adapter->GetDesc(&Desc);
        std::wstring Text{ L"***Adaptor***" };
        Text += Desc.Description;
        Text += L"\n";
        OutputDebugString(Text.c_str());

        UINT j = 0;
        IDXGIOutput* Output{ nullptr };

        while(Adapter->EnumOutputs(j,&Output) != DXGI_ERROR_NOT_FOUND){
            DXGI_OUTPUT_DESC Desc_{};
            Output->GetDesc(&Desc_);
            Text = L"***Output***";
            Text += Desc_.DeviceName;
            OutputDebugString(Text.c_str());

            UINT Count{ 0 };
            UINT Flags{ 0 };

            //카운트만 가져옴
            Output->GetDisplayModeList(m_dxgiBackBufferFormat, Flags, &Count, nullptr);
            std::vector<DXGI_MODE_DESC> ModeList(Count);

            Output->GetDisplayModeList(m_dxgiBackBufferFormat, Flags, &Count, &ModeList[0]);

            for (auto& x : ModeList) {
                UINT n = x.RefreshRate.Numerator;
                UINT d = x.RefreshRate.Denominator;
                Text = L"Width = " + std::to_wstring(x.Width) + L" ";
                Text += L"Height = " + std::to_wstring(x.Height) + L" ";
                Text += L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) + L"\n";

                ::OutputDebugString(Text.c_str());
                
                
            }

            ReleaseCom(Output);
            ++j;
        }




        AdapterList.push_back(Adapter);
        ++i;
    }

    for (size_t k = 0; k < AdapterList.size(); ++k) {
        ReleaseCom(AdapterList[k]);
    }

}
