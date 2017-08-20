#pragma once

#include <cstdint>
#include <fstream>
#include <memory>

#include "Common.h"

class SOUND_TOOLS_API SoundBuffer
{
public:
	SoundBuffer(
		size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
		const void* data, size_t dataSize);
	SoundBuffer(SoundBuffer&&);
	SoundBuffer(const SoundBuffer&) = delete;
	~SoundBuffer();

	size_t GetId() const;

	SoundBuffer& operator=(SoundBuffer&&);
	SoundBuffer& operator=(const SoundBuffer&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};