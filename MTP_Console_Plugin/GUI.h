#pragma once
#include <memory>
#include "pch.h"

class GUI
{
public:
	static inline bool init;
	static inline bool isShown;
	static void Initialize();
	static void DrawUI();
	static bool ImGuiWantCaptureMouse();
	static bool WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

