#include "JoyPad.h"
#include <assert.h>

// ���������R���g���[���̏����i�[���邽�߂̍\����
struct SearchJoyPadSet
{
	// DirectInput
	LPDIRECTINPUT8 dinput;
	// �e�R���g���[�����Ƃ̃f�o�C�X
	std::vector<LPDIRECTINPUTDEVICE8> joyPadDev;

	// �R���X�g���N�^
	SearchJoyPadSet()
	{
		dinput = NULL;
	}
};

//// ���̓I�u�W�F�N�g�������̃R�[���o�b�N
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
//	// ��������
//	return DIENUM_CONTINUE;
//}

// �R���g���[���������̃R�[���o�b�N
static BOOL CALLBACK EnumJoyPadCallBack(LPCDIDEVICEINSTANCE lpddi, void* pvRef)
{
	HRESULT ret;

	// �����̌^�������I�ɕϊ�
	SearchJoyPadSet* param = reinterpret_cast<SearchJoyPadSet*>(pvRef);

	LPDIRECTINPUTDEVICE8 dev;
	// �R���g���[�����ƂɃf�o�C�X�𐶐�
	ret = param->dinput->CreateDevice(
		lpddi->guidInstance,
		&dev,
		NULL);
	
	// ���s�Ȃ玟��
	if (ret != DI_OK)
		goto NEXT;

	// �f�[�^�`����ݒ�i�g���@�\���W���C�X�e�B�b�N�j
	ret = dev->SetDataFormat(&c_dfDIJoystick2);

	// ���s�Ȃ玟��
	if (ret != DI_OK)
		goto NEXT;

	// �z��ɒǉ�
	param->joyPadDev.push_back(dev);

NEXT:
	// ��������
	return DIENUM_CONTINUE;
}

JoyPad::JoyPad()
{
	m_pDInput = NULL;
}

JoyPad::~JoyPad()
{
	// �z��ɂ���f�o�C�X�����
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		SafeRelease(it->inputDevice);
	}
	m_joyPadSet.clear();
	// DirectInput�����
	SafeRelease(m_pDInput);
}

/// <summary>
/// ������
/// </summary>
/// <returns>����</returns>
bool JoyPad::Initialize(HWND window)
{
	// ����
	HRESULT ret;
	// �쐬
	ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_pDInput, NULL);

	if (ret != DI_OK)
		return false;

	// �󂯓n���p�p�����[�^
	SearchJoyPadSet param;
	param.dinput = m_pDInput;

	// ���p�\�ȃf�o�C�X��񋓂���
	ret = m_pDInput->EnumDevices(
		DI8DEVCLASS_GAMECTRL,
		EnumJoyPadCallBack,
		&param,
		DIEDFL_ATTACHEDONLY);

	if (ret != DI_OK)
		return false;
	
	// �f�o�C�X�z���ݒ�
	std::vector<LPDIRECTINPUTDEVICE8>::iterator it;
	for (it = param.joyPadDev.begin(); it != param.joyPadDev.end(); it++)
	{
		LPDIRECTINPUTDEVICE8 dev = *it;

		// �A�v�����A�N�e�B�u���́A�f�o�C�X��r���A�N�Z�X�ɐݒ�
		ret = dev->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		if (ret != DI_OK)
			continue;

		// �A�N�Z�X����v��
		ret = dev->Acquire();

		// ���s�Ȃ玟��
		if (ret != DI_OK)
			continue;

		// �f�o�C�X1���̏��
		JoyPadSet initPad;
		initPad.inputDevice = *it;
		// �z��ɒǉ�����
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

		// �����擾
		ret = it->inputDevice->Poll();
		if (ret != DI_OK)
		{
			ret = it->inputDevice->Acquire();
			// ���̓��X�g�̏ꍇ�́A�A�N�Z�X����v����������
			while (ret == DIERR_INPUTLOST)
				ret = it->inputDevice->Acquire();
		}

		// ���͍X�V
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
