#include "RunApplication.h"

#include <algorithm>
#include <exception>
#include <future>
#include <string>
#include <sstream>
#include <vector>

#include "ScopedThread.h"
#include "SoundTools/SoundDevice.h"
#include "SoundTools/SoundContext.h"
#include "SoundTools/SoundBuffer.h"
#include "SoundTools/SoundSource.h"
#include "SoundTools/WaveBuffer.h"
#include "ThreadSafeStreams.h"

namespace
{
	class SoundObject
	{
	public:
		SoundObject(const char* name) :
			m_name(name),
			m_buffer(WaveBuffer(name).MakeSoundBuffer())
		{
			m_source.SetBuffer(&m_buffer);
		}
		SoundObject(
			const char* name,
			const WaveBuffer& waveBuffer) :
			m_name(name),
			m_buffer(waveBuffer.MakeSoundBuffer())
		{
			m_source.SetBuffer(&m_buffer);
		}
		SoundObject(SoundObject&&) = default;
		SoundObject(const SoundObject&) = delete;

		const SoundBuffer& GetBuffer() const { return m_buffer; }
		SoundSource& GetSource() { return m_source; }
		const SoundSource& GetSource() const { return m_source; }

		const char* GetName() const { return m_name.c_str(); }

		SoundObject& operator=(SoundObject&&) = default;
		SoundObject& operator=(const SoundObject&) = delete;

	private:
		std::string m_name;
		SoundBuffer m_buffer;
		SoundSource m_source;
	};

	template<size_t n>
	struct SizeToType;

	template<>
	struct SizeToType<1>
	{
		using Type = uint8_t;
		using Bigger = uint16_t;
	};

	template<>
	struct SizeToType<2>
	{
		using Type = uint16_t;
		using Bigger = uint32_t;
	};

	template<size_t bytesPerSample>
	decltype(auto) ComputeNoteSample(float frequency, size_t nSample, size_t sampleRate)
	{
		using T = typename SizeToType<bytesPerSample>::Type;
		using Bigger = typename SizeToType<bytesPerSample>::Bigger;

		constexpr auto max = std::numeric_limits<T>::max();
		constexpr auto max_2f = static_cast<float>(max) * .5f;

		auto biggerResult = 
			static_cast<Bigger>(
				max_2f * (std::sin((nSample * frequency * 2.f * 3.14159f) / (sampleRate)) + 1.f));

		biggerResult = std::max<Bigger>(  0, biggerResult);
		biggerResult = std::min<Bigger>(max, biggerResult);

		return static_cast<T>(biggerResult);
	}


	template<size_t bytesPerSample>
	WaveBuffer MakeNoteBuffer(float frequency, float duration, size_t sampleRate)
	{
		using T = typename SizeToType<bytesPerSample>::Type;

		// Compute buffer size
		auto samplesCount = static_cast<size_t>(duration * sampleRate);
		auto bytesCount = samplesCount * bytesPerSample;

		// Allocate the buffer
		std::unique_ptr<uint8_t[]> buffer;
		buffer.reset(new uint8_t[bytesCount]);

		auto raw = reinterpret_cast<T*>(buffer.get());

		for (size_t i = 0; i < samplesCount; ++i)
		{
			raw[i] = ComputeNoteSample<bytesPerSample>(frequency, i, sampleRate);
		}

		return WaveBuffer(1, bytesPerSample * 8, sampleRate, std::move(buffer), bytesCount);
	}

	void RunApplicationSafe(ThreadSafeIStream& input, ThreadSafeOStream& output)
	{
		try
		{
			SoundDevice device;
			SoundContext context(&device);

			context.SetCurrent();

			std::vector<SoundObject> sounds;

			auto findSound = [&](const char* name)
				-> SoundObject*
			{
				for (auto& sound : sounds)
				{
					if (std::strcmp(sound.GetName(), name) == 0)
					{
						return &sound;
					}
				}

				return nullptr;
			};
			auto soundNotFoundMessage = [&](const char* name)
			{
				output << "\"" << name << "\"" << " is not found" << std::endl;
			};

			std::string tmp;
			std::string line;

			auto executeCommand = [&](const std::string& command)
			{
				std::stringstream lineStream(line);

				auto skipSpaces = [&lineStream]()
				{
					while (lineStream.peek() == ' ') lineStream.get();
				};

				lineStream >> tmp;
				skipSpaces();

				if (tmp == "play")
				{
					std::getline(lineStream, tmp);

					try
					{
						sounds.emplace_back(tmp.c_str());
						sounds.back().GetSource().Play();
					}
					catch (const std::exception& ex)
					{
						output << ex.what() << std::endl;
					}
				}
				else if (tmp == "pause")
				{
					std::getline(lineStream, tmp);
					auto sound = findSound(tmp.c_str());

					if (sound == nullptr)
					{
						soundNotFoundMessage(tmp.c_str());
					}
					else
					{
						sound->GetSource().Pause();
					}
				}
				else if (tmp == "stop")
				{
					std::getline(lineStream, tmp);
					auto sound = findSound(tmp.c_str());

					if (sound == nullptr)
					{
						soundNotFoundMessage(tmp.c_str());
					}
					else
					{
						sound->GetSource().Stop();
					}
				}
				else if (tmp == "state")
				{
					std::getline(lineStream, tmp);
					auto sound = findSound(tmp.c_str());

					if (sound == nullptr)
					{
						soundNotFoundMessage(tmp.c_str());
					}
					else
					{
						const char* stateStr = nullptr;
						switch (sound->GetSource().GetState())
						{
						case SoundSourceState::Initial:
							stateStr = "Initial";
							break;
						case SoundSourceState::Paused:
							stateStr = "Paused";
							break;
						case SoundSourceState::Playing:
							stateStr = "Playing";
							break;
						case SoundSourceState::Stopped:
							stateStr = "Stopped";
							break;
						}

						output << stateStr << std::endl;
					}
				}
				else if (tmp == "resume")
				{
					std::getline(lineStream, tmp);
					auto sound = findSound(tmp.c_str());

					if (sound == nullptr)
					{
						soundNotFoundMessage(tmp.c_str());
					}
					else
					{
						sound->GetSource().Play();
					}
				}
				else if (tmp == "repeat")
				{
					lineStream >> tmp;
					auto sound = findSound(tmp.c_str());

					if (sound == nullptr)
					{
						sounds.emplace_back(tmp.c_str());
						sound = &sounds.back();

						sound->GetSource().SetLooping(true);
						sound->GetSource().Play();
					}
					else
					{
						skipSpaces();
						std::getline(lineStream, tmp);
						std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
						sound->GetSource().SetLooping(tmp == "true");
					}
				}
				else if (tmp == "note")
				{
					float freq, duration;
					lineStream >> freq >> duration;

					std::stringstream sstr;
					sstr << freq << ' ' << duration;
					auto sname = sstr.str();

					auto sampleRate = 11025;
					auto buffer = MakeNoteBuffer<1>(freq, duration, sampleRate);
					sounds.emplace_back(sname.c_str(), buffer);
					auto sound = &sounds.back();

					sound->GetSource().Play();
				}
				else
				{
					system(line.c_str());
				}
			};

			ScopedThread scopedThread([&](bool& finished, std::mutex& mutex)
			{
				auto isFinished = [&]()
				{
					std::lock_guard<std::mutex> guard(mutex);
					return finished;
				};

				while (!isFinished())
				{
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(3s);

					sounds.erase(
						std::remove_if(sounds.begin(), sounds.end(),
							[&](const SoundObject& sound)
					{
						if (sound.GetSource().GetState() == SoundSourceState::Stopped)
						{
							output << "deleting " << sound.GetName() << std::endl;
							return true;
						}

						return false;
					}), sounds.end());
				}
			});


			input.GetLine(line);

			while (!line.empty())
			{
				executeCommand(line);
				input.GetLine(line);
			}
		}
		catch (const std::exception& ex)
		{
			output << "Exception: " << ex.what() << std::endl;
		}
	}
}

void RunApplication(std::istream& input, std::ostream& output)
{
	RunApplicationSafe(
		ThreadSafeIStream(input),
		ThreadSafeOStream(output));

	system("pause");
}