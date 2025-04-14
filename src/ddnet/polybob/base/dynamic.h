#pragma once

#include "detect.h"

#ifdef CONF_FAMILY_WINDOWS
#define DYNAMIC_EXPORT __declspec(dllexport)
#define DYNAMIC_IMPORT __declspec(dllimport)
#else
#define DYNAMIC_EXPORT
#define DYNAMIC_IMPORT
#endif
