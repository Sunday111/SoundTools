#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "SoundDevice.h"
#include "SoundContext.h"

#include <AL/al.h>
#include <AL/alc.h>

void main()
{
	try
	{
		SoundDevice device;
		SoundContext context(&device);
		context.SetCurrent();

		//auto wavFile = ReadWavFile("owl.wav");
		//
		//ALuint buffer;
		//alGenBuffers(1, &buffer);
		//
		//ALuint source;
		//alGenSources(1, &source);
		//alSourcef(source, AL_PITCH, 1);
		//alSourcef(source, AL_GAIN, 1);
		//alSource3f(source, AL_POSITION, 0, 0, 0);
		//alSource3f(source, AL_VELOCITY, 0, 0, 0);
		//alSourcei(source, AL_LOOPING, AL_FALSE);
		//
		//alBufferData(
		//	buffer,
		//	ToAlFormat(wavFile.format.numChannels, wavFile.format.bitsPerSample),
		//	wavFile.data.data.data(),
		//	wavFile.data.data.size(),
		//	wavFile.format.sampleRate);
		//
		//alSourcei(source, AL_BUFFER, buffer);
		//
		//alSourcePlay(source);
		//
		//using namespace std::chrono_literals;
		//std::this_thread::sleep_for(10000ms);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception: " << ex.what() << std::endl;
	}
}