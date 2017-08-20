#include <fstream>
#include <vector>

#include "OpenAlTools.h"

#include "SoundTools/SoundBuffer.h"

namespace
{
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

SoundBuffer::SoundBuffer(
	size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
	const void* data, size_t dataSize)
{
	m_d = std::make_unique<Impl>();

	// Make new OpenAL buffer
	OpenAlCallVoid(alGenBuffers, 1, &m_d->alBuffer);

	// Assign buffer data
	OpenAlCallVoid(alBufferData,
		m_d->alBuffer,
		ToAlFormat(channelsCount, bitsPerSample), data,
		static_cast<int>(dataSize),
		static_cast<int>(sampleRate));
}

SoundBuffer::SoundBuffer(SoundBuffer&&) = default;
SoundBuffer::~SoundBuffer() = default;
SoundBuffer& SoundBuffer::operator=(SoundBuffer&&) = default;

size_t SoundBuffer::GetId() const
{
	m_d->Check();
	return m_d->alBuffer;
}