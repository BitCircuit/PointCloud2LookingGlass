#pragma once
#ifndef UTIL_H
#define UTIL_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define osWindows
#elif __APPLE__
#define osMacOS
#elif __linux__
#define osLinux
#endif

#include "stdio.h"
#include "stdbool.h"
#include "stdexcept"
#include "filesystem"
#include "vector"
#include "fstream"
#include "iostream"
#include "String.h"
#include "tuple"
#include <chrono>
#include <thread>
#include "future"
#include "ctime"
#include "progressbar.hpp"

using namespace std;

//#define PATH_MAX_LEN	200

namespace Util {
	enum findMode { alpha, color };

	struct pathResult
	{
		string directory;
		bool isFile;
		string fileName;
		//string extension;
	};

	vector<string> scanFileByType(const char*, const char*);
	bool findInPLY(const char*, findMode);
	pathResult pathParser(const char*, const char*);
};


//char currentProgramPath[100] = "asd";		
//char plyFileDirectory[PATH_MAX_LEN] = "asd";			// dummy initialization
//char plyFileName[PATH_MAX_LEN] = "asd";
//bool isPathAFile = false;




#endif // !Util