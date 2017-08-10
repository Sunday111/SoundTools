#pragma once

#include <memory>

#include "Common.h"

class SOUND_TOOLS_API SoundDevice
{
public:
	SoundDevice(SoundDevice&& that);
	SoundDevice(const SoundDevice&) = delete;
	SoundDevice(const char* name = nullptr);
	~SoundDevice();

	void* GetHandle() const;

	SoundDevice& operator=(SoundDevice&& that);
	SoundDevice& operator=(const SoundDevice&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};