#pragma once

#include <memory>

class SoundDevice;

class SoundContext
{
public:
	SoundContext(SoundDevice* device);
	SoundContext(const SoundContext&) = delete;
	SoundContext(SoundContext&& that);
	~SoundContext();

	long GetHandle() const;
	void SetCurrent() const;

	SoundContext& operator=(SoundContext&&);
	SoundContext& operator=(const SoundContext&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};