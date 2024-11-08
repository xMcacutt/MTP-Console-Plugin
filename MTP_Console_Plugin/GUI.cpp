#include "pch.h"
#include <filesystem>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "pipe.h"
#include "GUI.h"

#include "TygerFrameworkAPI.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool GUI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (GUI::init){
        if (!GUI::isShown && msg == WM_KEYDOWN && (wParam == VK_F2 || wParam == VK_OEM_2)) {
            ImGuiIO& io = ImGui::GetIO();
            io.ClearEventsQueue();
            API::SetTyInputFlag(NoKeyboardInput, true);
            API::SetTyInputFlag(NoMouseInput, true);
            GUI::isShown = true;
            return true;
        }
        else if (GUI::isShown && msg == WM_KEYDOWN && (wParam == VK_F2 || wParam == VK_ESCAPE)) {
            pipe::Get()->ClearBuffer();
            ImGui::ClearActiveID();
            ImGuiIO& io = ImGui::GetIO();
            io.ClearEventsQueue();
            API::SetTyInputFlag(NoKeyboardInput, false);
            API::SetTyInputFlag(NoMouseInput, false);
            GUI::isShown = false;
            return true;
        }
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;
    }
    return false;
}

void GUI::Initialize() {

    //Setup ImGui Context
    ImGui::CreateContext();

    //Set the font to be the same as TygerFramework
    API::SetImGuiFont(ImGui::GetIO().Fonts);

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    if (API::GetTyWindowHandle() == 0) {
        API::LogPluginMessage("Error");
        return;
    }
    //Setup backend
    ImGui_ImplWin32_InitForOpenGL(API::GetTyWindowHandle());
    ImGui_ImplOpenGL3_Init();

    API::LogPluginMessage("Initialized ImGui");
    init = true;
}

//ImGui Drawing code
void GUI::DrawUI() {
    if (!GUI::init)
        GUI::Initialize();

    if (!GUI::isShown)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetWindowSize({ displaySize.x, 18});
    ImGui::SetNextWindowPos(ImVec2(18, displaySize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::Begin("MTP Command Line", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text(">_");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.0, 0.0, 0.0, 0.7f});        // Background color
    ImGui::SetKeyboardFocusHere();
    ImGui::InputText("##input", pipe::Get()->commandBuffer, IM_ARRAYSIZE(pipe::Get()->commandBuffer));
    ImGui::PopStyleColor();

    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        pipe::Get()->SendCommand();
        pipe::Get()->ClearBuffer();
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


bool GUI::ImGuiWantCaptureMouse() {
    return GUI::init && ImGui::GetIO().WantCaptureMouse;
}