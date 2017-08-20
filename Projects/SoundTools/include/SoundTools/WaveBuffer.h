#pragma once

#include <memory>

#include "Common.h"
#include "SoundBuffer.h"

class SOUND_TOOLS_API WaveBuffer
{
public:
	WaveBuffer(const char* filename);
	WaveBuffer(
		size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
		void* data, size_t dataSize);
	WaveBuffer(
		size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
		std::unique_ptr<uint8_t[]>&& data, size_t dataSize);
	WaveBuffer(WaveBuffer&&);
	WaveBuffer(const WaveBuffer&) = delete;

	SoundBuffer MakeSoundBuffer() const;
	void SaveToFile(const char* filename) const;

	WaveBuffer& operator=(WaveBuffer&&);
	WaveBuffer& operator=(const WaveBuffer&) = delete;

private:
	size_t m_channelsCount;
	size_t m_bitsPerSample;
	size_t m_sampleRate;
	size_t m_dataSize;
	std::unique_ptr<uint8_t[]> m_data;
};