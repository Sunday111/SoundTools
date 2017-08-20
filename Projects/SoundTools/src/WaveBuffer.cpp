#include <fstream>
#include <vector>

#include "SoundTools/WaveBuffer.h"

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
			std::unique_ptr<uint8_t[]> data;
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

		result.data.data.reset(new uint8_t[result.data.subchunk2Size]);
		file.read(reinterpret_cast<char*>(result.data.data.get()), result.data.subchunk2Size);

		return result;
	}
}

WaveBuffer::WaveBuffer(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open())
	{
		throw std::invalid_argument("Failed to open the file");
	}

	auto fileData = ReadWavFile(file);
	m_channelsCount = fileData.format.numChannels;
	m_bitsPerSample = fileData.format.bitsPerSample;
	m_sampleRate = fileData.format.sampleRate;
	m_dataSize = fileData.data.subchunk2Size;
	m_data = std::move(fileData.data.data);
}

WaveBuffer::WaveBuffer(
	size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
	void* data, size_t dataSize) :
	m_channelsCount(channelsCount),
	m_bitsPerSample(bitsPerSample),
	m_sampleRate(sampleRate),
	m_dataSize(dataSize)
{
	m_data.reset(new uint8_t[m_dataSize]);
	std::memcpy(m_data.get(), data, m_dataSize);
}

WaveBuffer::WaveBuffer(
	size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
	std::unique_ptr<uint8_t[]>&& data, size_t dataSize) :
	m_channelsCount(channelsCount),
	m_bitsPerSample(bitsPerSample),
	m_sampleRate(sampleRate),
	m_dataSize(dataSize),
	m_data(std::move(data))
{}

SoundBuffer WaveBuffer::MakeSoundBuffer() const
{
	return SoundBuffer(
		m_channelsCount,
		m_bitsPerSample,
		m_sampleRate,
		m_data.get(),
		m_dataSize);
}

void WaveBuffer::SaveToFile(const char* filename) const
{
	std::ofstream file(filename,
		std::ios::binary |
		std::ios::out |
		std::ios::trunc);

	if (!file.is_open())
	{
		throw std::runtime_error("Could not create the file");
	}

	WavFile wavData;

	// RIFF
	std::memcpy(&wavData.riff.chunkId, "RIFF", sizeof(wavData.riff.chunkId));
	wavData.riff.chunkSize = 36 + m_dataSize;
	std::memcpy(&wavData.riff.format, "WAVE", sizeof(wavData.riff.format));

	file.write(
		reinterpret_cast<const char*>(&wavData.riff),
		sizeof(wavData.riff));

	// FORMAT
	size_t sampleBytesCount = m_bitsPerSample / 8;
	std::memcpy(&wavData.format.subchunk1Id, "fmt ", sizeof(wavData.format.subchunk1Id));
	wavData.format.subchunk1Size = 16;
	wavData.format.audioFormat = 1;
	wavData.format.numChannels = m_channelsCount;
	wavData.format.sampleRate = m_sampleRate;
	wavData.format.byteRate = m_sampleRate * sampleBytesCount;
	wavData.format.blockAlign = m_channelsCount * sampleBytesCount;
	wavData.format.bitsPerSample = m_bitsPerSample;

	file.write(
		reinterpret_cast<const char*>(&wavData.format),
		sizeof(wavData.format));

	// Data
	file.write("data", 4);

	auto dataSize = static_cast<uint32_t>(m_dataSize);
	file.write(
		reinterpret_cast<const char*>(&dataSize),
		sizeof(dataSize));
	file.write(
		reinterpret_cast<const char*>(m_data.get()),
		m_dataSize);

	file.close();
}

WaveBuffer::WaveBuffer(WaveBuffer&&) = default;
WaveBuffer& WaveBuffer::operator=(WaveBuffer&&) = default;