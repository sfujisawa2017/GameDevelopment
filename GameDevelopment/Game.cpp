﻿//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <sstream>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "Resources\Basic.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() :
    m_window(0),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{

}

Game::~Game()
{
	ADX2Le::Finalize();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
	// 描画ステートを作成
	m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());
	// スプライトバッチを作成
	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
	// デバッグテキストを作成
	debugText = std::make_unique<DebugText>(m_d3dDevice.Get(), m_spriteBatch.get());

	// テクスチャファイルをロード
	ComPtr<ID3D11Resource> resource;
	//DX::ThrowIfFailed(
	//	CreateWICTextureFromFile(m_d3dDevice.Get(), L"Assets/pikatyu_s.png",
	//		resource.GetAddressOf(),
	//		m_texture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Resources/pikatyu_s.dds",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));
	// リソースをテクスチャ2Dに変換
	ComPtr<ID3D11Texture2D> cat;
	DX::ThrowIfFailed(resource.As(&cat));
	// 読み込んだテクスチャの情報を取得
	CD3D11_TEXTURE2D_DESC catDesc;
	cat->GetDesc(&catDesc);
	// スプライトの回転中心を指定
	m_origin.x = float(catDesc.Width / 2);
	m_origin.y = float(catDesc.Height / 2);
	// スプライトの表示位置を指定
	m_screenPos.x = 100.0f;
	m_screenPos.y = 100.0f;
	// キーボードユーティリティ作成
	keyboardUtil = std::make_unique<KeyboardUtil>();

	// マウスオブジェクト作成
	mouseUtil = std::make_unique<MouseUtil>(window);

	// サウンドファイル初期化
	ADX2Le::Initialize("Resources/ADX2_samples.acf");
	ADX2Le::LoadAcb("Resources/Basic.acb", "Resources/Basic.awb");
	ADX2Le::Play(CRI_BASIC_MUSIC2);

	// ゲームパッド
	gamepad = std::make_unique<GamePad>();
	gamepadTracker = std::make_unique<GamePad::ButtonStateTracker>();

	ctrMode = CTR_MODE_1;

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Resources/attack.dds",
			resource.GetAddressOf(),
			m_textureAttack.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> tex;
	CD3D11_TEXTURE2D_DESC texDesc;

	DX::ThrowIfFailed(resource.As(&tex));
	tex->GetDesc(&texDesc);
	m_originAttack.x = float(texDesc.Width / 2);
	m_originAttack.y = float(texDesc.Height / 2);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Resources/guard.dds",
			resource.GetAddressOf(),
			m_textureGuard.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(resource.As(&tex));
	tex->GetDesc(&texDesc);
	m_originGuard.x = float(texDesc.Width / 2);
	m_originGuard.y = float(texDesc.Height / 2);

	m_screenPos = Vector2(m_outputWidth/2.0f, m_outputHeight/2.0f);

	joyPad = std::make_unique<JoyPad>();
	joyPad->Initialize(m_window);
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

	ADX2Le::Update();

	joyPad->Update();

    // TODO: Add your game logic here.
    elapsedTime;

	// キーボード更新
	keyboardUtil->Update();
	// マウス更新
	mouseUtil->Update();

	//// ホイールクリック中は相対、それ以外なら絶対座標モードをセット
	//if (mouseUtil->IsPressed(MouseUtil::Button::Middle))
	//{
	//	mouseUtil->SetMode(Mouse::Mode::MODE_RELATIVE);
	//}
	//else
	//{
	//	mouseUtil->SetMode(Mouse::Mode::MODE_ABSOLUTE);
	//}

	//// マウスモードの表示
	//if (mouseUtil->GetMode() == DirectX::Mouse::Mode::MODE_ABSOLUTE)
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 100), L"Mouse Mode: Absolute");
	//}
	//else
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 100), L"Mouse Mode: Relative");
	//}

	//// マウス座標の表示
	//XMINT2 mousePos = mouseUtil->GetPos();
	//debugText->AddText(SimpleMath::Vector2(10, 200), L"Mouse X:%03d Y:%03d", mousePos.x, mousePos.y);

	//// マウスボタン状態を表示
	//if (mouseUtil->IsPressed(MouseUtil::Button::Left))
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 400), L"Left Button is Pushed", 1.0f);
	//}

	//if (mouseUtil->IsPressed(MouseUtil::Button::Middle))
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 400), L"Middle Button is Pushed", 1.0f);
	//}

	//if (mouseUtil->IsPressed(MouseUtil::Button::Right))
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 400), L"Right Button is Pushed", 1.0f);
	//}

	//if (mouseUtil->IsTriggered(MouseUtil::Button::Left))
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 500), L"Left Button is Triggerd", 1.0f);
	//}

	//if (mouseUtil->IsTriggered(MouseUtil::Button::Middle))
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 500), L"Middle Button is Triggerd", 1.0f);
	//}

	//if (mouseUtil->IsTriggered(MouseUtil::Button::Right))
	//{
	//	debugText->AddText(SimpleMath::Vector2(10, 500), L"Right Button is Triggerd", 1.0f);
	//}

	//m_screenPos = Vector2(mousePos.x, mousePos.y);

	// ゲームパッド０の状態取得
	GamePad::State state = gamepad->GetState(0);
	if (state.IsConnected())
	{
		gamepadTracker->Update(state);

		if (gamepadTracker->back == GamePad::ButtonStateTracker::PRESSED)
		{
			ctrMode = (CTR_MODE)(CTR_MODE_2 - ctrMode);
		}

		debugText->AddText(SimpleMath::Vector2(10, 400), L"Mode::%d", (int)(ctrMode+1));
	}

	auto caps = gamepad->GetCapabilities(0);

	if (state.IsConnected())
	{
		bool* attack;
		bool* guard;

		switch (ctrMode)
		{
		case CTR_MODE_1:
			attack = &state.buttons.a;
			guard = &state.buttons.b;
			break;
		case CTR_MODE_2:
			attack = &state.buttons.b;
			guard = &state.buttons.a;
			break;
		}

		m_attack = *attack;
		m_guard = *guard;

		if (m_attack)
		{
			debugText->AddText(SimpleMath::Vector2(10, 500), L"Attack");
		}

		if (m_guard)
		{
			debugText->AddText(SimpleMath::Vector2(10, 500), L"Guard");
		}
	}
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    // TODO: Add your rendering code here.
	// スプライトの描画
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->AlphaBlend());
	//m_spriteBatch->Draw(m_texture.Get(), m_screenPos, nullptr, Colors::White,
	//	XM_PI, m_origin, 1.0f);

	if (m_attack)
	{
		m_spriteBatch->Draw(m_textureAttack.Get(), m_screenPos, nullptr, Colors::White,
			0.0f, m_originAttack, 1.0f);
	}
	if (m_guard)
	{
		m_spriteBatch->Draw(m_textureGuard.Get(), m_screenPos, nullptr, Colors::White,
			0.0f, m_originGuard, 1.0f);
	}
	m_spriteBatch->End();

	// デバッグ用文字表示
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->Opaque());
	debugText->Draw();
	m_spriteBatch->End();

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels (see code below related to 11.1 fallback handling).
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
        );

    if (hr == E_INVALIDARG)
    {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it.
        hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            &featureLevels[1],
            _countof(featureLevels) - 1,
            D3D11_SDK_VERSION,
            m_d3dDevice.ReleaseAndGetAddressOf(),
            &m_featureLevel,
            m_d3dContext.ReleaseAndGetAddressOf()
            );
    }

    DX::ThrowIfFailed(hr);

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    // DirectX 11.1 if present
    if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
        (void)m_d3dContext.As(&m_d3dContext1);

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        ComPtr<IDXGIFactory2> dxgiFactory2;
        if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
        {
            // DirectX 11.1 or later

            // Create a descriptor for the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = backBufferWidth;
            swapChainDesc.Height = backBufferHeight;
            swapChainDesc.Format = backBufferFormat;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = backBufferCount;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
            fsSwapChainDesc.Windowed = TRUE;

            // Create a SwapChain from a Win32 window.
            DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
                m_d3dDevice.Get(),
                m_window,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                m_swapChain1.ReleaseAndGetAddressOf()
                ));

            DX::ThrowIfFailed(m_swapChain1.As(&m_swapChain));
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.BufferDesc.Width = backBufferWidth;
            swapChainDesc.BufferDesc.Height = backBufferHeight;
            swapChainDesc.BufferDesc.Format = backBufferFormat;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = m_window;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Windowed = TRUE;

            DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
        }

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain1.Reset();
    m_swapChain.Reset();
    m_d3dContext1.Reset();
    m_d3dContext.Reset();
    m_d3dDevice1.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}