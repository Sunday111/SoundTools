#include "RunApplication.h"

#include <exception>
#include <string>
#include <thread>
#include <vector>

#include "SoundDevice.h"
#include "SoundContext.h"
#include "SoundBuffer.h"
#include "SoundSource.h"

void RunApplication(std::istream& input, std::ostream& output)
{
	try
	{
		SoundDevice device;
		SoundContext context(&device);

		context.SetCurrent();

		std::vector<SoundBuffer> buffers;
		std::vector<SoundSource> sources;

		std::string line;
		std::getline(input, line);

		while (!line.empty())
		{
			try
			{
				SoundBuffer buffer(line.c_str());
				SoundSource source;

				source.SetBuffer(&buffer);
				source.Play();

				buffers.push_back(std::move(buffer));
				sources.push_back(std::move(source));
			}
			catch (const std::exception& ex)
			{
				output << ex.what() << std::endl;
			}

			std::getline(input, line);
		}
	}
	catch (const std::exception& ex)
	{
		output << "Exception: " << ex.what() << std::endl;
	}
}