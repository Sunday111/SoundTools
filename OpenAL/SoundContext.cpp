#include "SoundDevice.h"

#include <stdexcept>

#include "OpenAlTools.h"

#include "SoundContext.h"

class SoundContext::Impl
{
public:
	~Impl()
	{
		if (context != nullptr)
		{
			alcDestroyContext(context);
		}
	}

	ALCdevice* GetDevice() const
	{
		return reinterpret_cast<ALCdevice*>(device->GetHandle());
	}

	SoundDevice* device;
	ALCcontext* context;
};

SoundContext::SoundContext(SoundDevice* device)
{
	if (device == nullptr)
	{
		throw std::invalid_argument("Could not initialize context with null device");
	}

	m_d = std::make_unique<Impl>();
	m_d->device = device;
	m_d->context = alcCreateContext(m_d->GetDevice(), nullptr);
}

SoundContext::SoundContext(SoundContext&& that) = default;
SoundContext::~SoundContext() = default;
SoundContext& SoundContext::operator=(SoundContext&&) = default;

void* SoundContext::GetHandle() const
{
	return m_d->context;
}

void SoundContext::SetCurrent() const
{
	OpenAlCall(alcMakeContextCurrent, m_d->context);
}