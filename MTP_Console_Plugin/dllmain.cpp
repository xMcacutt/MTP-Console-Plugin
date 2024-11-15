#include "pch.h"
#include "TygerFrameworkAPI.hpp"
#include "pipe.h"
#include "GUI.h"
#include <Windows.h>
#include "core.h"
#include "hero.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void TickBeforeGame(float deltaSeconds) {
	GUI::DrawUI();
}

extern "C" __declspec(dllexport) bool TygerFrameworkPluginInitialize(TygerFrameworkPluginInitializeParam * param) {
	if (!API::Initialize(param))
		return false;

	API::AddPluginWndProc((WndProcFunc)GUI::WndProc);
	API::AddPluginImGuiWantCaptureMouse((ImGuiWantCaptureMouseFunc)GUI::ImGuiWantCaptureMouse);
	API::AddTickBeforeGame(TickBeforeGame);

	pipe::Get()->Initialize();

	return true;
}