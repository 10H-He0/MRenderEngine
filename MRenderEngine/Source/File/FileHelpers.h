#pragma once

#include <string>
#include <filesystem>
#include <Utils/FormatConvert.h>

class MFileHelpers
{
public:
	static bool IsFileExit(const std::wstring& FileName)
	{
		return std::filesystem::exists(FileName);
	}

	static std::wstring EnginDir()
	{
		std::wstring EngineDir = TFormatConvert::StrToWStr(std::string(SOLUTION_DIR)) + L"MRenderEngine/";

		return EngineDir;
	}
};