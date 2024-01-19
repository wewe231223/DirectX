#pragma once
enum class KEY_STATE {
	/// <summary>
	///  nothing state 
	/// </summary>
	NONE,

	/// <summary>
	///  pressed on time 
	/// </summary>
	DOWN,

	/// <summary>
	///  pressing during time 
	/// </summary>
	PRESS,

	/// <summary>
	///  released on time  
	/// </summary>
	RELEASE
};

enum class MOUSE_BUTTON {
	LEFT,
	RIGHT,
	MIDDLE,
	SIDE
};

class Input {
private:
	Input();
	~Input();

	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;

private:
	static Input* m_instance;
public:
	static Input* GetInstance();

private:

	KEY_STATE* m_keyboardState{ nullptr };
	KEY_STATE* m_mouseState{ nullptr };

	IDirectInput8W* m_pdirectInput{};
	IDirectInputDevice8W* m_pkeyDevice{};
	IDirectInputDevice8W* m_pmouseDevice{};

	LONG m_nDeltaMouseX{};
	LONG m_nDeltaMouseY{};


public:
	const KEY_STATE GetKey(int key) const;
	const KEY_STATE GetMouseButton(int key) const;
	const KEY_STATE GetMouseButton(MOUSE_BUTTON key) const;
	const LONG GetDeltaMouseX() const;
	const LONG GetDeltaMouseY() const;
public:
	void Init(HWND hWnd,HINSTANCE Instance);
	void Update();
	void Terminate();

	bool m_notfired = true;
};

#define INPUT Input::GetInstance()