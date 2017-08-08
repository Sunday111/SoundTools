#include <cassert>
#include <fstream>
#include <vector>

#include "OpenAlTools.h"

#include "SoundBuffer.h"

namespace
{
	struct WavFile
	{
		struct
		{
			char chunkId[4];
			uint32_t chunkSize;
			char format[4];
		} riff;

		struct
		{
			char subchunk1Id[4];
			uint32_t subchunk1Size;
			uint16_t audioFormat;
			uint16_t numChannels;
			uint32_t sampleRate;
			uint32_t byteRate;
			uint16_t blockAlign;
			uint16_t bitsPerSample;
		} format;

		struct
		{
			char subchunk2Id[4];
			uint32_t subchunk2Size;
			std::vector<uint8_t> data;
		} data;
	};

	WavFile ReadWavFile(const char* name)
	{
		WavFile result;

		std::ifstream file("owl.wav", std::ios::binary);
		assert(file.is_open());

		file.read(reinterpret_cast<char*>(&result.riff), sizeof(result.riff));
		assert(strncmp(result.riff.chunkId, "RIFF", 4) == 0);
		assert(strncmp(result.riff.format, "WAVE", 4) == 0);

		file.read(reinterpret_cast<char*>(&result.format), sizeof(result.format));
		assert(strncmp(result.format.subchunk1Id, "fmt ", 4) == 0);

		file.read(reinterpret_cast<char*>(&result.data.subchunk2Id), sizeof(result.data.subchunk2Id));
		assert(strncmp(result.data.subchunk2Id, "data", 4) == 0);

		file.read(reinterpret_cast<char*>(&result.data.subchunk2Size), sizeof(result.data.subchunk2Size));

		result.data.data.resize(result.data.subchunk2Size);
		file.read(reinterpret_cast<char*>(&result.data.data[0]), result.data.subchunk2Size);

		return result;
	}

	ALenum ToAlFormat(short channels, short samples)
	{
		bool stereo = (channels > 1);

		switch (samples)
		{
		case 16: return stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		case 8:  return stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
		}

		return -1;
	}
}

class SoundBuffer::Impl
{
public:
	ALuint alBuffer;
};

SoundBuffer::SoundBuffer() :
	m_d(std::make_unique<Impl>())
{}
SoundBuffer::SoundBuffer(SoundBuffer&&) = default;
SoundBuffer::~SoundBuffer() = default;
SoundBuffer& SoundBuffer::operator=(SoundBuffer&&) = default;

SoundBuffer SoundBuffer::LoadFromWavFile(const char* filename)
{
	auto wavFile = ReadWavFile(filename);

	SoundBuffer result;
	// Make new OpenAL buffer
	OpenAlCall(alGenBuffers, 1, &result.m_d->alBuffer);

	// Assign buffer data
	OpenAlCall(alBufferData,
		result.m_d->alBuffer,
		ToAlFormat(wavFile.format.numChannels, wavFile.format.bitsPerSample),
		static_cast<const void*>(wavFile.data.data.data()),
		static_cast<int>(wavFile.data.data.size()),
		static_cast<int>(wavFile.format.sampleRate));
}