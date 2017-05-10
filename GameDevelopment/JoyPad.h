//---------------------------------------------
//
// JoyPadクラス
//
// JoyPad（コントローラ）を扱うクラス
//---------------------------------------------
#pragma once

// DirectInputのバージョンを指定
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>

template<class T>
void SafeRelease(T*& p) {
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

class JoyPad
{
public:
	// コンストラクタ
	JoyPad();
	// デストラクタ
	~JoyPad();
	// 初期化
	bool Initialize(HWND window);
	// 更新
	void Update();
private:
	// DirectInputインスタンス
	LPDIRECTINPUT8 m_pDInput;

	struct JoyPadSet {

		JoyPadSet() {
			inputDevice = NULL;
		}

		// デバイス
		LPDIRECTINPUTDEVICE8 inputDevice;
		// 入力情報
		DIJOYSTATE2	joypad;
		// 前回の入力情報
		DIJOYSTATE2	joypadOld;
	};

	// コントローラ配列
	std::vector<JoyPadSet> m_joyPadSet;
};

