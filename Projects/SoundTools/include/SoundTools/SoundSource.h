#pragma once

#include <memory>

#include "Common.h"

enum class SoundSourceState
{
	Initial,
	Playing,
	Paused,
	Stopped
};

class SoundBuffer;

class SOUND_TOOLS_API SoundSource
{
public:
	SoundSource();
	SoundSource(SoundSource&&);
	SoundSource(const SoundSource&) = delete;
	~SoundSource();

	void SetBuffer(SoundBuffer* buffer);
	void Pause() const;
	void Play() const;
	void Stop() const;
	SoundSourceState GetState() const;

	SoundSource& operator=(SoundSource&&);
	SoundSource& operator=(const SoundSource&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};