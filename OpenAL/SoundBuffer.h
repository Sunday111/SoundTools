#pragma once

#include <cstdint>
#include <fstream>
#include <memory>

class SoundBuffer
{
public:
	SoundBuffer(const char* fileName);
	SoundBuffer(std::ifstream& file);
	SoundBuffer(SoundBuffer&&);
	SoundBuffer(const SoundBuffer&) = delete;
	~SoundBuffer();

	size_t GetId() const;

	SoundBuffer& operator=(SoundBuffer&&);
	SoundBuffer& operator=(const SoundBuffer&) = delete;

private:
	class Impl;
	std::unique_ptr<Impl> m_d;
};