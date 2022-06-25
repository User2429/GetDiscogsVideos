#pragma once

//	Disable deprecation of gmtime, localtime, strcat and sprintf.
//	These are considered unsafe by Microsoft but their replacements (gmtime_s, etc)
//	are not supported by the (Linux) GNU Compiler so cannot be used for cross-platform development.
#define _CRT_SECURE_NO_WARNINGS 1

#include <charconv>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
