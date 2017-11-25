/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/
#pragma once

#include <iostream>
#include <string>
#include "windows.h"
#include <experimental\filesystem>
#include <vector>

#define WORKING_DIRECTORY std::experimental::filesystem::current_path().string()

class Utility {

public:
	std::vector<std::string> fileLister (std::string dirName, std::string fileType = "*") {
		std::vector<std::string> fileList;
		std::string path = dirName + "\\" + fileType;
		std::cout << std::experimental::filesystem::current_path() << std::endl;
		WIN32_FIND_DATA search_data;

		memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

		// convert to LPCWSTR
		std::wstring stemp = std::wstring(path.begin(), path.end());
		LPCWSTR sw = stemp.c_str();
		HANDLE handle = FindFirstFile(sw, &search_data);

		while (handle != INVALID_HANDLE_VALUE)
		{
			wchar_t* fname = search_data.cFileName;
			std::wstring ws(fname);
			std::string str(ws.begin(), ws.end());
			fileList.push_back(str);

			if (FindNextFile(handle, &search_data) == FALSE)
				break;
		}

		//Close the handle after use or memory/resource leak
		FindClose(handle);

		return fileList;
	}

	float degToRad(float degree)
	{
		const float radian = (3.14159f / 180.0f) * degree;
	}

	float radToDegree(float radian)
	{
		const float degree = (180.0f / 3.14159f) * degree;
	}
};