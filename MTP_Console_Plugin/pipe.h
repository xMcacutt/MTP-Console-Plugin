#pragma once
#include <string>
#include <memory>
#include "pch.h"

class pipe
{
private:
	static void AttemptConnection();
	static void CheckConnection();
public:
	static std::shared_ptr<pipe>& Get();
	void Initialize();
	void ClearBuffer();
	void SendCommand();
	bool isConnected;
	bool attemptingConnection;
	char commandBuffer[256] = "";
};