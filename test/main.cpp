
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

#include "../src/writer.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CREATE: {
      writer::Init(hwnd);

      SetTimer(hwnd, 1, 100, NULL);
      return 0;
    }
    case WM_TIMER:
      InvalidateRect(hwnd, NULL, FALSE);  // Mark entire window for redraw
      return 0;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      writer::BeginDraw();
      writer::Clear(writer::Colors::Black);

      writer::SetTextColor(writer::Colors::White);
      writer::DrawText("Test 2", 20, 0, 100, 20);

      writer::SetTextColor(writer::Color::Orange);
      writer::DrawText("Test 2", 20, 100, 100, 30);

      writer::EndDraw();
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_DESTROY: {
      writer::Release();
      PostQuitMessage(0);
      return 0;
    }
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main() {
  HINSTANCE hInstance = GetModuleHandle(0);

  const wchar_t CLASS_NAME[] = L"topbarclass";

  WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);

  RegisterClass(&wc);

  HWND hwnd = CreateWindowEx(
      0, CLASS_NAME, L"Topbar2", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, hInstance, NULL);

  if (!hwnd) return 0;

  ShowWindow(hwnd, TRUE);
  UpdateWindow(hwnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
