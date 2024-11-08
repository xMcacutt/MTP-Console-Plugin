#include "pipe.h"
#include <windows.h>
#include <iostream>
#include "TygerFrameworkAPI.hpp"

const char* pipeName = R"(\\.\pipe\MulTyCommandPipe)";
static HANDLE hPipe;

std::shared_ptr<pipe>& pipe::Get() {
    static auto instance = std::make_shared<pipe>();
    return instance;
}

void pipe::Initialize() {
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)pipe::AttemptConnection, NULL, 0, nullptr);
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)pipe::CheckConnection, NULL, 0, nullptr);
}

void pipe::CheckConnection() {
    const char* heartbeatMessage = "PING\n";
    DWORD bytesWritten;
    while (true) {

        if (pipe::Get()->isConnected && !WriteFile(hPipe, heartbeatMessage, strlen(heartbeatMessage), &bytesWritten, NULL))
            break;
        Sleep(1000);
    }
    CloseHandle(hPipe);
    hPipe = INVALID_HANDLE_VALUE;
    pipe::Get()->isConnected = false;
    API::LogPluginMessage("Pipe disconnected.");
    pipe::Get()->Initialize();
}

void pipe::ClearBuffer() {
    commandBuffer[0] = '\0';
}

void pipe::AttemptConnection() {
    pipe::Get()->attemptingConnection = true;
    do
    {
        hPipe = CreateFileA(
            pipeName,              // Pipe name
            GENERIC_WRITE,         // Desired access (write access)
            0,                     // No sharing
            NULL,                  // Default security attributes
            OPEN_EXISTING,         // Opens existing pipe
            0,                     // Default attributes
            NULL);                 // No template file
        Sleep(100);
    } while (hPipe == INVALID_HANDLE_VALUE);
    API::LogPluginMessage("Connected to pipe");
    pipe::Get()->attemptingConnection = false;
    pipe::Get()->isConnected = true;
}

void pipe::SendCommand() {
    if (hPipe == INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        API::LogPluginMessage("Pipe disconnected.");
        isConnected = false;
        Initialize();
        return;
    }

    DWORD bytesWritten;

    std::string commandWithNewline = std::string(commandBuffer) + '\n';

    bool success = WriteFile(
        hPipe,                     // Pipe handle
        commandWithNewline.c_str(), // Message to write (with newline)
        commandWithNewline.length(),// Message length (with newline)
        &bytesWritten,             // Number of bytes written
        NULL);                     // Not overlapped

    if (success) {
        API::LogPluginMessage("Command sent.");
        FlushFileBuffers(hPipe);
    }
    if (!success) {
        DWORD error = GetLastError();
        API::LogPluginMessage(std::to_string(error));
        CloseHandle(hPipe);
        API::LogPluginMessage("Pipe disconnected.");
        isConnected = false;
        Initialize();
        return;
    }
}
