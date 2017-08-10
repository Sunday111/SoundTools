#include "OpenAlTools.h"
#include "SoundTools/SoundBuffer.h"
#include "SoundTools/SoundSource.h"

class SoundSource::Impl
{
public:
	Impl() :
		sourceId(alInvalidId)
	{}

	~Impl()
	{
		if (IdIsValid())
		{
			OpenAlCallVoid(alDeleteSources, 1, (const ALuint*)&sourceId);
			sourceId = alInvalidId;
		}
	}

	void Check() const
	{
		if (!IdIsValid())
		{
			throw std::runtime_error("Trying to using an empty or deleted sound source");
		}
	}

	bool IdIsValid() const
	{
		return sourceId != alInvalidId;
	}

	ALuint sourceId;
};

SoundSource::SoundSource() :
	m_d(std::make_unique<Impl>())
{
	OpenAlCallVoid(alGenSources, 1, &m_d->sourceId);
	alSourcef(m_d->sourceId, AL_PITCH, 1);
	alSourcef(m_d->sourceId, AL_GAIN, 1);
	alSource3f(m_d->sourceId, AL_POSITION, 0, 0, 0);
	alSource3f(m_d->sourceId, AL_VELOCITY, 0, 0, 0);
	alSourcei(m_d->sourceId, AL_LOOPING, AL_FALSE);
}

SoundSource::SoundSource(SoundSource&&) = default;
SoundSource::~SoundSource() = default;
SoundSource& SoundSource::operator=(SoundSource&&) = default;


void SoundSource::SetBuffer(SoundBuffer* buffer)
{
	m_d->Check();
	OpenAlCallVoid(alSourcei,
		m_d->sourceId,
		(ALenum)AL_BUFFER,
		(ALint)buffer->GetId());
}

void SoundSource::Play() const
{
	m_d->Check();
	OpenAlCallVoid(alSourcePlay, m_d->sourceId);
}