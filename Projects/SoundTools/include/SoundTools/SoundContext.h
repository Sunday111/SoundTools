#pragma once

#include <memory>

#include "Common.h"

class SoundDevice;

class SOUND_TOOLS_API SoundContext
{
public:
	SoundContext(SoundDevice* device);
	SoundContext(const SoundContext&) = delete;
	SoundContext(SoundContext&& that);
	~SoundContext();

	void* GetHandle() const;
	void SetCurrent() const;

	SoundContext& operator=(SoundContext&&);
	SoundContext& operator=(const SoundContext&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};