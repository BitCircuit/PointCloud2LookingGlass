#pragma once
#ifndef UTIL_H
#define UTIL_H

#include "stdio.h"
#include "filesystem"
#include "vector"
#include "fstream"
#include "iostream"
#include "String.h"
#include "progressbar.hpp"

enum findMode { alpha, color };

std::vector<std::string> scanFileByType(char*, const char*);

bool findInPLY(char*, findMode);

#endif // !Util