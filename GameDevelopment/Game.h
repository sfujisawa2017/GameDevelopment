//
// Game.h
//

#pragma once

#include <CommonStates.h>
#include <Mouse.h>
#include <GamePad.h>
#include "StepTimer.h"
#include "DebugText.h"
#include "KeyboardUtil.h"
#include "MouseUtil.h"
#include "ADX2Le.h"
#include "JoyPad.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game();

	virtual ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext1;

    Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DebugText> debugText;
	std::unique_ptr<KeyboardUtil> keyboardUtil;
	std::unique_ptr<MouseUtil> mouseUtil;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;

	std::unique_ptr<DirectX::GamePad> gamepad;
	std::unique_ptr<DirectX::GamePad::ButtonStateTracker> gamepadTracker;

	enum CTR_MODE
	{
		CTR_MODE_1,
		CTR_MODE_2,
	};

	CTR_MODE ctrMode;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureAttack;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureGuard;
	DirectX::SimpleMath::Vector2 m_originAttack;
	DirectX::SimpleMath::Vector2 m_originGuard;
	bool m_attack;
	bool m_guard;

	std::unique_ptr<JoyPad> joyPad;
};