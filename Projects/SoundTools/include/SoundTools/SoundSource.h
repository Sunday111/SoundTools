#pragma once

#include <memory>

#include "Common.h"

class SoundBuffer;

class SOUND_TOOLS_API SoundSource
{
public:
	SoundSource();
	SoundSource(SoundSource&&);
	SoundSource(const SoundSource&) = delete;
	~SoundSource();

	void SetBuffer(SoundBuffer* buffer);
	void Play() const;

	SoundSource& operator=(SoundSource&&);
	SoundSource& operator=(const SoundSource&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};