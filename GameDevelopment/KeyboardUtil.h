#pragma once

#include <WinSDKVer.h>
#include <Keyboard.h>

// キーボードユーティリティクラス
class KeyboardUtil
{
	using Keys = DirectX::Keyboard::Keys;
public:
	// コンストラクタ
	KeyboardUtil();
	// 毎フレーム更新
	void Update();
	// 指定キーが押されているか
	bool IsPressed(Keys key);
	// 指定キーのトリガーチェック
	bool IsTriggered(Keys key);
	// 指定キーのリリースチェック
	bool IsReleased(Keys key);
protected:
	// キーボード
	std::unique_ptr<DirectX::Keyboard> m_Keyboard;
	// ステート
	DirectX::Keyboard::State m_State;
	// キーボードトラッカー
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
};