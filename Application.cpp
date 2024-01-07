#include "pch.h"
#include "Timer.h"
#include "Input.h"
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
#include "Scene.h"

DirectXApplication::DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName): Application(hInstance, wcpWindowName) {
    m_timer = std::make_unique<Timer>();
    m_timer->Reset();
    m_timer->Start();
    INPUT->Init(m_hWnd,m_hInstance);
    Initialize();
    ThrowIfFailed(m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr));
    m_pScene = std::make_unique<Scene>();
    m_pScene->Initialize(m_d3dDevice, m_d3dCommandList);
    m_pScene->Set4xMsaaState(m_b4xMsaaState, m_n4xMsaaQuality);

    ThrowIfFailed(m_d3dCommandList->Close());
    ID3D12CommandList* CommandLists[] = { m_d3dCommandList.Get() };
    m_d3dCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);
    FlushCommandQueue();
}

DirectXApplication::DirectXApplication(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW pWindowProperties){
}

DirectXApplication::~DirectXApplication(){
    if (m_d3dDevice != nullptr) {
        FlushCommandQueue();
    }
}
// 프로시저
LRESULT DirectXApplication::Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            m_bPaused = true;
            m_timer->Stop();
        }
        else {
        
            m_bPaused = false;
            m_timer->Start();
        }
        break;
    case WM_SIZE:
        m_nClientWidth = LOWORD(lParam);
        m_nClientHeight = HIWORD(lParam);

        if (m_d3dDevice) {
            switch(wParam){
            case SIZE_MINIMIZED:
                m_bPaused = true;
                m_bMinimized = true;
                m_bMaximized = true;
                break;
            case SIZE_MAXIMIZED:
                m_bPaused = false;
                m_bMinimized = false;
                m_bMaximized = true;
                Resize();
                break;
            case SIZE_RESTORED:
                if (m_bMinimized) {
                    m_bPaused = false;
                    m_bMinimized = false;
                    Resize();
                }
                else if (m_bMaximized) {
                    m_bPaused = false;
                    m_bMaximized = false;
                    Resize();
                }
                else if (m_bResizing) {
                    // 사이즈 변경 중에는 일단 Resize() 를 호출하지 않기로 한다
                }
                else {
                    Resize();
                }
                break;
            default:
                break;
            }
        }
        break;
    case WM_ENTERSIZEMOVE:
        m_bPaused = true;
        m_bResizing = true;
        m_timer->Stop();
        break;
    case WM_EXITSIZEMOVE:
        m_bPaused = false;
        m_bResizing = false;
        m_timer->Start();
        // 사이즈 변경이 끝나면 버퍼를 조정 
        Resize();
        break;
    case WM_MENUCHAR:
        // Accel Handle 을 사용하지 않으므로 이와 같은 입력에 반응하지 않도록 한다 
        break;
        //return MAKELRESULT(0, MNC_CLOSE);
     
    case WM_GETMINMAXINFO:
        //윈도우  최소크기 지정 
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
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
    while (m_mMsg.message != WM_QUIT) {
        if (::PeekMessage(&m_mMsg, 0, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&m_mMsg);
            ::DispatchMessage(&m_mMsg);            
        }
        else {
            Update(0.f);
            Render();
        }
    }
    return INPUT->Terminate();
}

void DirectXApplication::Update(float fDeltaTime){
    m_timer->Update();
    m_timer->SetFPSWindowTitle(m_hWnd);
    INPUT->Update();
}

void DirectXApplication::Render(){
    ThrowIfFailed(m_d3dCommandAllocator->Reset());
    ThrowIfFailed(m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), m_pScene->GetPipeLineStateObject().Get()));
    m_d3dCommandList->RSSetViewports(1, &m_d3dScreenViewPort);
    m_d3dCommandList->RSSetScissorRects(1, &m_d3dSissorRect);

    CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffer[m_nCurrentBackBuffer].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_d3dCommandList->ResourceBarrier(1, &ResourceBarrier);
    // 백 버퍼를 LightSteelBlue 색으로 초기화
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView = GetCurrentBackBufferView();
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView = GetDepthStencilView();
    m_d3dCommandList->ClearRenderTargetView(CurrentBackBufferView, DirectX::Colors::LightSteelBlue, 0, nullptr);
    m_d3dCommandList->ClearDepthStencilView(DepthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

    CurrentBackBufferView = GetCurrentBackBufferView();
    DepthStencilView = GetDepthStencilView();
    m_d3dCommandList->OMSetRenderTargets(1, &CurrentBackBufferView, true, &DepthStencilView);

    // Scene 의 그리기 연산 
    m_pScene->Render();

    ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3dSwapChainBuffer[m_nCurrentBackBuffer].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_d3dCommandList->ResourceBarrier(1, &ResourceBarrier);

    ThrowIfFailed(m_d3dCommandList->Close());
    ID3D12CommandList* CommandLists[] = { m_d3dCommandList.Get() };
    m_d3dCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

    ThrowIfFailed(m_dxgiSwapChain->Present(0,0));

    m_nCurrentBackBuffer = (m_nCurrentBackBuffer + 1) % SWAPCHAINBUFFERCOUNT;
    FlushCommandQueue();
    
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
    CreateSwapChain();
    CreateRenderTargetViewDescriptorHeap();
    CreateDepthStencilViewDescriptorHeap();


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
    RenderTargetViewDesc.NumDescriptors = SWAPCHAINBUFFERCOUNT;
    RenderTargetViewDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    RenderTargetViewDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    RenderTargetViewDesc.NodeMask = 0;
    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&RenderTargetViewDesc, IID_PPV_ARGS(m_d3dRenderTargetViewHeap.GetAddressOf())));
}

void DirectXApplication::CreateDepthStencilViewDescriptorHeap(){
    D3D12_DESCRIPTOR_HEAP_DESC DepthStencilViewDescriptorHeap{};
    DepthStencilViewDescriptorHeap.NumDescriptors = 1;
    DepthStencilViewDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    DepthStencilViewDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DepthStencilViewDescriptorHeap.NodeMask = 0;
    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&DepthStencilViewDescriptorHeap, IID_PPV_ARGS(m_d3dDepthStencilViewHeap.GetAddressOf())));
}


bool DirectXApplication::Resize(){
    FlushCommandQueue();

    ThrowIfFailed(m_d3dCommandList->Reset(m_d3dCommandAllocator.Get(), nullptr));
    for (UINT i = 0; i < SWAPCHAINBUFFERCOUNT; ++i) {
        m_d3dSwapChainBuffer[i].Reset();
    }
    m_d3dDepthStencilBuffer.Reset();

    ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(SWAPCHAINBUFFERCOUNT, m_nClientWidth, m_nClientHeight, m_dxgiBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    m_nCurrentBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHeapHandle( m_d3dRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart() );
    for (UINT i = 0; i < SWAPCHAINBUFFERCOUNT; ++i) {
        ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_d3dSwapChainBuffer[i])));
        m_d3dDevice->CreateRenderTargetView(m_d3dSwapChainBuffer[i].Get(), nullptr, RenderTargetViewHeapHandle);
        RenderTargetViewHeapHandle.Offset(1, m_nRenderTargetViewDescriptorSize);
    }
    
    D3D12_RESOURCE_DESC DepthStencilDesc;
    DepthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    DepthStencilDesc.Alignment = 0;
    DepthStencilDesc.Width = m_nClientWidth;
    DepthStencilDesc.Height = m_nClientHeight;
    DepthStencilDesc.DepthOrArraySize = 1;
    DepthStencilDesc.MipLevels = 1;
    DepthStencilDesc.Format = m_dxgiDepthStencilFormat;
    DepthStencilDesc.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
    DepthStencilDesc.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;
    DepthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE ClearOption{};
    ClearOption.Format = m_dxgiDepthStencilFormat;
    ClearOption.DepthStencil.Depth = 1.f;
    ClearOption.DepthStencil.Stencil = 0;

    CD3DX12_HEAP_PROPERTIES HeapProperties{ D3D12_HEAP_TYPE_DEFAULT };
    ThrowIfFailed(
        m_d3dDevice->CreateCommittedResource(
            &HeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &DepthStencilDesc, 
            D3D12_RESOURCE_STATE_COMMON, 
            &ClearOption,
            IID_PPV_ARGS(m_d3dDepthStencilBuffer.GetAddressOf()))
    );

    m_d3dDevice->CreateDepthStencilView(m_d3dDepthStencilBuffer.Get(), nullptr, m_d3dDepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart());

    CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3dDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_d3dCommandList->ResourceBarrier(1, &ResourceBarrier);

    ThrowIfFailed(m_d3dCommandList->Close());
    ID3D12CommandList* CommandLists[] = { m_d3dCommandList.Get() };
    m_d3dCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);

    FlushCommandQueue();

    m_d3dScreenViewPort.TopLeftX = 0;
    m_d3dScreenViewPort.TopLeftY = 0;
    m_d3dScreenViewPort.Width = static_cast<float>(m_nClientWidth);
    m_d3dScreenViewPort.Height = static_cast<float>(m_nClientHeight);
    m_d3dScreenViewPort.MaxDepth = 0.f;
    m_d3dScreenViewPort.MinDepth = 1.f;

    m_d3dSissorRect = D3D12_RECT{ 0,0,m_nClientWidth,m_nClientHeight };


    return true;
}

void DirectXApplication::FlushCommandQueue(){
    m_nCurrentFence++;
    ThrowIfFailed(m_d3dCommandQueue->Signal(m_d3dFence.Get(), m_nCurrentFence));

    if (m_d3dFence->GetCompletedValue() < m_nCurrentFence) {
        HANDLE EventHandle = CreateEventExW(nullptr, NULL, NULL, EVENT_ALL_ACCESS);
        ThrowIfFailed(m_d3dFence->SetEventOnCompletion(m_nCurrentFence, EventHandle));
        ::WaitForSingleObject(EventHandle, INFINITE);
        ::CloseHandle(EventHandle);
    }
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

ID3D12Resource* DirectXApplication::GetCurrentBackBuffer() const {
    return m_d3dSwapChainBuffer[m_nCurrentBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXApplication::GetCurrentBackBufferView() const {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
    m_d3dRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(),
        m_nCurrentBackBuffer,
        m_nRenderTargetViewDescriptorSize
    );
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXApplication::GetDepthStencilView() const {
    return m_d3dDepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();
}
