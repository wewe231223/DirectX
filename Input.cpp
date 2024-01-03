#include "pch.h"
#include "Input.h"
Input* Input::m_instance{ nullptr };

Input::Input() {
}

Input::~Input(){

}

/// <summary>
/// Get Key State
/// </summary>
/// <param name="key"> : Should DIK_... to Get Accurate Keystate</param>
/// <returns>KEY_STATE enum</returns>
const KEY_STATE Input::GetKey(int key) const{
	return m_keyboardState[key];
}

const KEY_STATE Input::GetMouseButton(int key) const{
	return m_mouseState[key];
}

const KEY_STATE Input::GetMouseButton(MOUSE_BUTTON key) const{
	return m_mouseState[static_cast<int>(key)];
}

void Input::Init(HWND hWnd, HINSTANCE Instance) {
	m_keyboardState = new KEY_STATE[256];
	m_mouseState = new KEY_STATE[3];
	for (auto i = 0; i < 256; ++i) {
		m_keyboardState[i] = KEY_STATE::NONE;
	}
	for (auto i = 0; i < 3; ++i) {
		m_mouseState[i] = KEY_STATE::NONE;
	}
	HRESULT hr = S_OK;

	ThrowIfFailed(hr = DirectInput8Create(Instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pdirectInput, NULL));
	
	ThrowIfFailed(hr = m_pdirectInput->CreateDevice(GUID_SysKeyboard, &m_pkeyDevice, NULL))
	m_pkeyDevice->SetDataFormat(&c_dfDIKeyboard);
	ThrowIfFailed(hr = m_pkeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY));
	while (m_pkeyDevice->Acquire() == DIERR_INPUTLOST);

	ThrowIfFailed(hr = m_pdirectInput->CreateDevice(GUID_SysMouse, &m_pmouseDevice, NULL));
	m_pmouseDevice->SetDataFormat(&c_dfDIMouse);
	ThrowIfFailed(hr = m_pmouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	while (m_pmouseDevice->Acquire() == DIERR_INPUTLOST);
}

Input* Input::GetInstance(){
	if (m_instance == nullptr) {
		m_instance = new Input();
	}
	return m_instance;
}

void Input::Update() {
	BYTE Keystate[256]{};
	DIMOUSESTATE Mousestate{};
	HRESULT hr{};

	if (FAILED(hr = m_pkeyDevice->GetDeviceState(256, Keystate))) {
		while (m_pkeyDevice->Acquire() == DIERR_INPUTLOST);
	}
	if (FAILED(hr = m_pmouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &Mousestate))) {
		while (m_pmouseDevice->Acquire() == DIERR_INPUTLOST);
	}
	if (Keystate[DIK_ESCAPE] & 0x80) {
		PostQuitMessage(0);
	}


	for (UINT i = 0; i < 256; ++i) {
		if (Keystate[i] & 0x80) {
			if (m_keyboardState[i] == KEY_STATE::NONE or m_keyboardState[i] == KEY_STATE::RELEASE) {
				m_keyboardState[i] = KEY_STATE::DOWN;
			}
			else if (m_keyboardState[i] == KEY_STATE::DOWN) {
				m_keyboardState[i] = KEY_STATE::PRESS;
			}
		}
		else {
			if (m_keyboardState[i] == KEY_STATE::PRESS or m_keyboardState[i] == KEY_STATE::DOWN) {
				m_keyboardState[i] = KEY_STATE::RELEASE;
			}
			else if (m_keyboardState[i] == KEY_STATE::RELEASE) {
				m_keyboardState[i] = KEY_STATE::NONE;
			}
		}
	}
	/*
	* 0 = Left Mouse Button 
	* 1 = Right Mouse Button 
	* 2 = Middle Mouse Button 
	* 3 = Side Mouse Button( 하지만 이 키는 마우스별로 인식되는 버튼이 다를 수 있다. 따라서 이 키는 되도록 사용하지 말 것 ) 
	*/
	for (UINT i = 0; i < 4; ++i) {
		if (Mousestate.rgbButtons[i] & 0x80) {
			if (m_mouseState[i] == KEY_STATE::NONE or m_mouseState[i] == KEY_STATE::RELEASE) {
				m_mouseState[i] = KEY_STATE::DOWN;
				printf("%d = DOWN\n", i);
			}
			else if (m_mouseState[i] == KEY_STATE::DOWN) {
				m_mouseState[i] = KEY_STATE::PRESS;
				printf("%d = PRESS\n", i);
			}
		}
		else {
			if (m_mouseState[i] == KEY_STATE::PRESS or m_mouseState[i] == KEY_STATE::DOWN) {
				m_mouseState[i] = KEY_STATE::RELEASE;
				printf("%d = RELEASE\n", i);
			}
			else if (m_mouseState[i] == KEY_STATE::RELEASE) {
				m_mouseState[i] = KEY_STATE::NONE;
			}
		}
	}
	
	/*
	* 위에서 아래로 Y+ 아래에서 위로 Y-
	* 왼쪽에서 오른쪽으로 X+ 오른쪽에서 왼쪽으로 X-
	*/
	
	m_nDeltaMouseX = Mousestate.lX;
	m_nDeltaMouseY = Mousestate.lY;

}

void Input::Terminate(){
	if (m_keyboardState) delete[] m_keyboardState;
	m_keyboardState = nullptr;

	if (m_mouseState) delete[] m_mouseState;
	m_mouseState = nullptr;

	if (m_pmouseDevice) {
		m_pmouseDevice->Unacquire();
		m_pmouseDevice->Release();
		m_pmouseDevice = NULL;
	}

	if (m_pkeyDevice) {
		m_pkeyDevice->Unacquire();
		m_pkeyDevice->Release();
		m_pkeyDevice = NULL;
	}

	if (m_pdirectInput) {
		m_pdirectInput->Release();
		m_pdirectInput = NULL;
	}
	delete Input::GetInstance();
}

