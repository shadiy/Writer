
#include <intsafe.h>
#define UNICODE
#define _UNICODE

#include <comdef.h>  // For _com_error (optional for debugging)
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <libloaderapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <wrl.h>
#include <wrl/client.h>

#include <string>

#include "writer.h"

using Microsoft::WRL::ComPtr;

int window_width = 1024;
int window_height = 768;

ComPtr<ID2D1Factory> pD2DFactory;
ComPtr<IDWriteFactory> pDWriteFactory;
ComPtr<ID2D1HwndRenderTarget> pRenderTarget;
ComPtr<IDWriteTextFormat> pTextFormat;
ComPtr<ID2D1SolidColorBrush> pBrush;

std::wstring stringToWstring(const std::string &s) {
  int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
  std::wstring ws(len, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws[0], len);
  return ws;
}

std::string wstringToString(const std::wstring &wstr) {
  int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0,
                                       nullptr, nullptr);
  std::string str(bufferSize - 1, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize,
                      nullptr, nullptr);
  return str;
}

void writer::Init(HWND hwnd) {
  CoInitialize(NULL);

  HRESULT hr = D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr,
      reinterpret_cast<void **>(pD2DFactory.GetAddressOf()));
  if (FAILED(hr)) {
    printf("ERROR: Failed creating D2D1 factory\n");
    return;
  }

  hr = DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
      reinterpret_cast<IUnknown **>(pDWriteFactory.GetAddressOf()));
  if (FAILED(hr)) {
    printf("ERROR: Failed creating DWrite factory\n");
    return;
  }

  LoadFont("Segoe UI", 12);

  RECT rc;
  GetClientRect(hwnd, &rc);

  D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
  D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps =
      D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right, rc.bottom));

  hr = pD2DFactory->CreateHwndRenderTarget(&props, &hwndProps, &pRenderTarget);
  if (FAILED(hr)) {
    printf("ERROR: Failed creating render target\n");
    return;
  }

  hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),
                                            &pBrush);
  if (FAILED(hr)) {
    printf("ERROR: Failed creating solid color brush\n");
  }
}

void writer::SetTextColor(writer::Color color) { pBrush->SetColor(color); }
void writer::SetTextColor(writer::Colors color) {
  pBrush->SetColor(writer::Color(color));
}

// Loads System Fonts (ex. Segoe UI)
void writer::LoadFont(std::string name, float fontSize) {
  pTextFormat.Reset();

  std::wstring wname = stringToWstring(name);

  HRESULT hr = pDWriteFactory->CreateTextFormat(
      wname.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-us",
      &pTextFormat);

  if (FAILED(hr)) {
    printf("ERROR: Failed loading font %s\n", name.c_str());
  }
}

void writer::Release() {
  pBrush.Reset();
  pRenderTarget.Reset();
  pTextFormat.Reset();
  pDWriteFactory.Reset();
  pD2DFactory.Reset();

  CoUninitialize();
}

writer::TextMetrics writer::MeasureText(std::wstring text) {
  DWRITE_TEXT_METRICS metrics;

  ComPtr<IDWriteTextLayout> layout;
  HRESULT hr = pDWriteFactory->CreateTextLayout(
      text.c_str(), static_cast<UINT32>(text.length()), pTextFormat.Get(),
      1920.0f, static_cast<FLOAT>(window_height), &layout);

  hr = layout->GetMetrics(&metrics);

  return metrics;
}

writer::TextMetrics writer::MeasureText(std::string text) {
  return MeasureText(stringToWstring(text));
}

void writer::DrawText(std::wstring text, int x, int y, float width,
                      float height) {
  ComPtr<IDWriteTextLayout> textLayout;
  HRESULT hr = pDWriteFactory->CreateTextLayout(
      text.c_str(), static_cast<UINT32>(text.length()), pTextFormat.Get(),
      width, height, &textLayout);

  if (FAILED(hr)) {
    printf("ERROR: Failed creating text layout\n");
    return;
  }

  pRenderTarget->DrawTextLayout(D2D1::Point2F(x, y), textLayout.Get(),
                                pBrush.Get());
}

void writer::DrawText(std::string text, int x, int y, float width,
                      float height) {
  std::wstring wtext = stringToWstring(text);
  DrawText(wtext, x, y, width, height);
}

void writer::BeginDraw() { pRenderTarget->BeginDraw(); }
void writer::Clear(writer::Color color) { pRenderTarget->Clear(color); }
void writer::Clear(writer::Colors color) {
  pRenderTarget->Clear(writer::Color(color));
}
void writer::EndDraw() { pRenderTarget->EndDraw(); }
