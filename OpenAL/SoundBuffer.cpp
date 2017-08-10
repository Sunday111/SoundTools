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

	WavFile ReadWavFile(std::ifstream& file)
	{
		WavFile result;

		auto check = [](bool cond, const char* message)
		{
			if (!cond)
			{
				throw std::invalid_argument(message);
			}
		};

		auto invalidFileFormatMessage = "Invalid file format";

		file.read(reinterpret_cast<char*>(&result.riff), sizeof(result.riff));
		check(strncmp(result.riff.chunkId, "RIFF", 4) == 0, invalidFileFormatMessage);
		check(strncmp(result.riff.format, "WAVE", 4) == 0, invalidFileFormatMessage);

		file.read(reinterpret_cast<char*>(&result.format), sizeof(result.format));
		check(strncmp(result.format.subchunk1Id, "fmt ", 4) == 0, invalidFileFormatMessage);
		check(result.format.audioFormat == 1, invalidFileFormatMessage);

		file.read(reinterpret_cast<char*>(&result.data.subchunk2Id), sizeof(result.data.subchunk2Id));
		check(strncmp(result.data.subchunk2Id, "data", 4) == 0, invalidFileFormatMessage);

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

		throw std::invalid_argument("Unexpected format");
	}
}

class SoundBuffer::Impl
{
public:
	Impl() :
		alBuffer(alInvalidId)
	{
	}

	~Impl()
	{
		if (BufferIsValid())
		{
			OpenAlCallVoid(alDeleteBuffers, 1, (const ALuint*)&alBuffer);
			alBuffer = alInvalidId;
		}
	}

	bool BufferIsValid() const
	{
		return alBuffer != alInvalidId;
	}

	void Check() const
	{
		if (!BufferIsValid())
		{
			throw std::runtime_error("Trying to use deleted or invalid OpenAL buffer");
		}
	}

	ALuint alBuffer;
};

SoundBuffer::SoundBuffer(const char* filename) :
	SoundBuffer(std::ifstream(filename, std::ios::binary))
{}
SoundBuffer::SoundBuffer(std::ifstream& file)
{
	if (!file.is_open())
	{
		throw std::invalid_argument("Could not open the file");
	}

	m_d = std::make_unique<Impl>();

	auto wavFile = ReadWavFile(file);

	// Make new OpenAL buffer
	OpenAlCallVoid(alGenBuffers, 1, &m_d->alBuffer);

	// Assign buffer data
	OpenAlCallVoid(alBufferData,
		m_d->alBuffer,
		ToAlFormat(wavFile.format.numChannels, wavFile.format.bitsPerSample),
		static_cast<const void*>(wavFile.data.data.data()),
		static_cast<int>(wavFile.data.data.size()),
		static_cast<int>(wavFile.format.sampleRate));
}
SoundBuffer::SoundBuffer(SoundBuffer&&) = default;
SoundBuffer::~SoundBuffer() = default;
SoundBuffer& SoundBuffer::operator=(SoundBuffer&&) = default;

size_t SoundBuffer::GetId() const
{
	m_d->Check();
	return m_d->alBuffer;
}