#pragma once

#ifndef SOUND_TOOLS_LIB
	#define SOUND_TOOLS_API __declspec(dllexport)
#else
	#define SOUND_TOOLS_API __declspec(dllimport)
#endif