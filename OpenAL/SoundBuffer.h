#pragma once

#include <cstdint>
#include <memory>

class SoundBuffer
{
public:
	static SoundBuffer LoadFromWavFile(const char* filename);

	SoundBuffer(SoundBuffer&&);
	SoundBuffer(const SoundBuffer&) = delete;
	~SoundBuffer();

	size_t GetId() const;

	SoundBuffer& operator=(SoundBuffer&&);
	SoundBuffer& operator=(const SoundBuffer&) = delete;

private:
	SoundBuffer();

	class Impl;
	std::unique_ptr<Impl> m_d;
};