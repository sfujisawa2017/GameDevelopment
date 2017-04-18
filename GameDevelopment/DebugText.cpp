#include "DebugText.h"

using namespace DirectX;

DebugText::DebugText(ID3D11Device* d3dDevice, DirectX::SpriteBatch* spriteBatch)
{
	this->d3dDevice = d3dDevice;
	this->spriteBatch = spriteBatch;

	// フォントファイルの読み込み
	spriteFont = std::make_unique<SpriteFont>(d3dDevice, L"myfile.spritefont");

}

void DebugText::Draw()
{
	// 全ての文字列を描画する
	std::vector<DebugText::Text>::iterator it;
	for (it = textArray.begin(); it != textArray.end(); it++)
	{
		spriteFont->DrawString(spriteBatch, it->str.c_str(), it->pos);
	}
	textArray.clear();
}

void DebugText::AddText(const DirectX::XMFLOAT2 & position, LPCWSTR format, ...)
{
	DebugText::Text text;
	// 表示位置
	text.pos = position;	

	// 書式文字列を処理
	va_list args;

	va_start(args, format);
	// 出力用文字列の生成
	text.str.resize(_vscwprintf(format, args) + 1);    // +1 for '\0'
	// 出力用文字列の書き込み
	int length = _vsnwprintf_s(&text.str[0], text.str.size(), _TRUNCATE, format, args);

	va_end(args);

	// 文字列の配列に追加
	textArray.push_back(text);
}
