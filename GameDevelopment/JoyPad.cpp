#include "JoyPad.h"
#include <assert.h>

// 検索したコントローラの情報を格納するための構造体
struct SearchJoyPadSet
{
	// DirectInput
	LPDIRECTINPUT8 dinput;
	// 各コントローラごとのデバイス
	std::vector<LPDIRECTINPUTDEVICE8> joyPadDev;

	// コンストラクタ
	SearchJoyPadSet()
	{
		dinput = NULL;
	}
};

//// 入力オブジェクト検索時のコールバック
//static BOOL CALLBACK EnumObjectCallBack(LPCDIDEVICEOBJECTINSTANCE pInstance, void* pvRef)
//{
//	LPDIRECTINPUTDEVICE8 dev = (LPDIRECTINPUTDEVICE8)pvRef;
//	// For axes that are returned, set the DIPROP_RANGE property for the
//	// enumerated axis in order to scale min/max values.
//	if (pInstance->dwType & DIDFT_AXIS)
//	{
//		DIPROPRANGE diprg;
//		diprg.diph.dwSize = sizeof(DIPROPRANGE);
//		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
//		diprg.diph.dwHow = DIPH_BYID;
//		diprg.diph.dwObj = pInstance->dwType; // Specify the enumerated axis
//		diprg.lMin = -1000;
//		diprg.lMax = +1000;
//
//		// Set the range for the axis
//		if (FAILED(dev->SetProperty(DIPROP_RANGE, &diprg.diph)))
//			return DIENUM_STOP;
//	}
//
//	// 次を検索
//	return DIENUM_CONTINUE;
//}

// コントローラ検索時のコールバック
static BOOL CALLBACK EnumJoyPadCallBack(LPCDIDEVICEINSTANCE lpddi, void* pvRef)
{
	HRESULT ret;

	// 引数の型を強制的に変換
	SearchJoyPadSet* param = reinterpret_cast<SearchJoyPadSet*>(pvRef);

	LPDIRECTINPUTDEVICE8 dev;
	// コントローラごとにデバイスを生成
	ret = param->dinput->CreateDevice(
		lpddi->guidInstance,
		&dev,
		NULL);
	
	// 失敗なら次へ
	if (ret != DI_OK)
		goto NEXT;

	// データ形式を設定（拡張機能つきジョイスティック）
	ret = dev->SetDataFormat(&c_dfDIJoystick2);

	// 失敗なら次へ
	if (ret != DI_OK)
		goto NEXT;

	// 配列に追加
	param->joyPadDev.push_back(dev);

NEXT:
	// 次を検索
	return DIENUM_CONTINUE;
}

JoyPad::JoyPad()
{
	m_pDInput = NULL;
}

JoyPad::~JoyPad()
{
	// 配列にあるデバイスを解放
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		SafeRelease(it->inputDevice);
	}
	m_joyPadSet.clear();
	// DirectInputを解放
	SafeRelease(m_pDInput);
}

/// <summary>
/// 初期化
/// </summary>
/// <returns>成否</returns>
bool JoyPad::Initialize(HWND window)
{
	// 結果
	HRESULT ret;
	// 作成
	ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_pDInput, NULL);

	if (ret != DI_OK)
		return false;

	// 受け渡し用パラメータ
	SearchJoyPadSet param;
	param.dinput = m_pDInput;

	// 利用可能なデバイスを列挙する
	ret = m_pDInput->EnumDevices(
		DI8DEVCLASS_GAMECTRL,
		EnumJoyPadCallBack,
		&param,
		DIEDFL_ATTACHEDONLY);

	if (ret != DI_OK)
		return false;
	
	// デバイス配列を設定
	std::vector<LPDIRECTINPUTDEVICE8>::iterator it;
	for (it = param.joyPadDev.begin(); it != param.joyPadDev.end(); it++)
	{
		LPDIRECTINPUTDEVICE8 dev = *it;

		// アプリがアクティブ時は、デバイスを排他アクセスに設定
		ret = dev->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		if (ret != DI_OK)
			continue;

		// アクセス権を要求
		ret = dev->Acquire();

		// 失敗なら次へ
		if (ret != DI_OK)
			continue;

		// デバイス1個分の情報
		JoyPadSet initPad;
		initPad.inputDevice = *it;
		// 配列に追加する
		m_joyPadSet.push_back(initPad);
	}

	return true;
}

void JoyPad::Update()
{
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		HRESULT ret;

		// 情報を取得
		ret = it->inputDevice->Poll();
		if (ret != DI_OK)
		{
			ret = it->inputDevice->Acquire();
			// 入力ロストの場合は、アクセス件を要求し続ける
			while (ret == DIERR_INPUTLOST)
				ret = it->inputDevice->Acquire();
		}

		// 入力更新
		it->joypadOld = it->joypad;
		ret = it->inputDevice->GetDeviceState(sizeof(DIJOYSTATE2), &it->joypad);

		if (ret != DI_OK) continue;

		if ((BYTE)(it->joypad.rgbButtons[0] & 0x80) == 0)
		{
			static int a = 0;
			a++;
		}

	}
}
