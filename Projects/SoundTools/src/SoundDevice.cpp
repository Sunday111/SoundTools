#include <cassert>
#include <stdexcept>

#include "OpenAlTools.h"

#include "SoundTools/SoundDevice.h"

class SoundDevice::Impl
{
public:
	~Impl()
	{
		if (device != nullptr)
		{
			auto closeResult = alcCloseDevice(device);
			assert(closeResult);
		}
	}

	ALCdevice* device;
};

SoundDevice::SoundDevice(SoundDevice&& that) = default;
SoundDevice::SoundDevice(const char* name) :
	m_d(std::make_unique<Impl>())
{
	m_d->device = alcOpenDevice(name);

	if (m_d->device == nullptr)
	{
		throw std::runtime_error("Failed to initialize sound device");
	}
}
SoundDevice::~SoundDevice() = default;

void* SoundDevice::GetHandle() const
{
	return m_d->device;
}

SoundDevice& SoundDevice::operator=(SoundDevice&& that) = default;