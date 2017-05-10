//---------------------------------------------
//
// JoyPad�N���X
//
// JoyPad�i�R���g���[���j�������N���X
//---------------------------------------------
#pragma once

// DirectInput�̃o�[�W�������w��
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
	// �R���X�g���N�^
	JoyPad();
	// �f�X�g���N�^
	~JoyPad();
	// ������
	bool Initialize(HWND window);
	// �X�V
	void Update();
private:
	// DirectInput�C���X�^���X
	LPDIRECTINPUT8 m_pDInput;

	struct JoyPadSet {

		JoyPadSet() {
			inputDevice = NULL;
		}

		// �f�o�C�X
		LPDIRECTINPUTDEVICE8 inputDevice;
		// ���͏��
		DIJOYSTATE2	joypad;
		// �O��̓��͏��
		DIJOYSTATE2	joypadOld;
	};

	// �R���g���[���z��
	std::vector<JoyPadSet> m_joyPadSet;
};

