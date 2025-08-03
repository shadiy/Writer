#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <intsafe.h>
#include <windows.h>

#include <string>

namespace writer {

using Color = D2D1::ColorF;
using Colors = D2D1::ColorF::Enum;
using TextMetrics = DWRITE_TEXT_METRICS;

void Init(HWND hwnd);
void LoadFont(std::string name, float fontSize);
void BeginDraw();
void Clear(Color color);
void Clear(Colors color);
void SetTextColor(Color color);
void SetTextColor(Colors color);
TextMetrics MeasureText(std::wstring text);
TextMetrics MeasureText(std::string text);
void DrawText(std::wstring text, int x, int y, float width, float height);
void DrawText(std::string text, int x, int y, float width, float height);
void EndDraw();
void Release();

}  // namespace writer
