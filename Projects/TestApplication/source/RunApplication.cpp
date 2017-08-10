#include "RunApplication.h"

#include <exception>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "SoundTools/SoundDevice.h"
#include "SoundTools/SoundContext.h"
#include "SoundTools/SoundBuffer.h"
#include "SoundTools/SoundSource.h"

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
		std::string tmp;
		std::getline(input, line);

		while (!line.empty())
		{
			std::stringstream lineStream(line);

			lineStream >> tmp;

			if (tmp == "open")
			{
				while (lineStream.peek() == ' ') lineStream.get();

				std::getline(lineStream, tmp);

				try
				{
					SoundBuffer buffer(tmp.c_str());
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
			}
			else
			{
				system(line.c_str());
			}

			std::getline(input, line);
		}
	}
	catch (const std::exception& ex)
	{
		output << "Exception: " << ex.what() << std::endl;
	}
}