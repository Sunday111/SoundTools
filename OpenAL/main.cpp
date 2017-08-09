#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "SoundDevice.h"
#include "SoundContext.h"
#include "SoundBuffer.h"
#include "SoundSource.h"

#include <AL/al.h>
#include <AL/alc.h>

void main()
{
	try
	{
		SoundDevice device;
		SoundContext context(&device);

		context.SetCurrent();

		SoundBuffer buffer = SoundBuffer::LoadFromWavFile("cat_yowl_x.wav");
		SoundSource source;

		source.SetBuffer(&buffer);
		source.Play();

		std::cin.get();
		std::cin.get();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception: " << ex.what() << std::endl;
	}

}