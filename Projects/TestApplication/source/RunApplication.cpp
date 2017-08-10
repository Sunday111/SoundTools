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
#include "ThreadSafeStreams.h"

namespace
{
	class SoundObject
	{
	public:
		SoundObject(const char* name) :
			m_name(name),
			m_buffer(name)
		{
			m_source.SetBuffer(&m_buffer);
		}
		SoundObject(SoundObject&&) = default;
		SoundObject(const SoundObject&) = delete;

		const SoundBuffer& GetBuffer() const { return m_buffer; }
		const SoundSource& GetSource() const { return m_source; }

		const char* GetName() const { return m_name.c_str(); }

		SoundObject& operator=(SoundObject&&) = default;
		SoundObject& operator=(const SoundObject&) = delete;

	private:
		std::string m_name;
		SoundBuffer m_buffer;
		SoundSource m_source;
	};

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
				lineStream >> tmp;
				while (lineStream.peek() == ' ') lineStream.get();

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