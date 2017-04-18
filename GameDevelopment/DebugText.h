#pragma once

#include <wrl/client.h>
#include <SpriteFont.h>
#include <SimpleMath.h>
#include <vector>

// デバッグ用画面表示クラス
class DebugText
{
public:
	// テキスト1個分のデータ
	struct Text
	{
		// 表示位置
		DirectX::XMFLOAT2	pos;
		// 表示文字列
		std::wstring	str;
	};

	// コンストラクタ
	DebugText(ID3D11Device*	d3dDevice, DirectX::SpriteBatch* spriteBatch);
	// 描画
	// ※必ずSpriteBatchのBegin()とEnd()の間で呼び出すこと。
	void Draw();
	// テキスト追加（フォーマット指定付き）
	void AddText(const DirectX::XMFLOAT2& position, LPCWSTR format, ...);
protected:
	// デバイス
	ID3D11Device*	d3dDevice;
	// スプライトバッチ
	DirectX::SpriteBatch*	spriteBatch;
	// スプライトフォント
	std::unique_ptr<DirectX::SpriteFont>	spriteFont;
	// 文字列の配列
	std::vector<DebugText::Text> textArray;
};
