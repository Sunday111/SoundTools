#include <iostream>
#include <exception>

#include "SoundDevice.h"
#include "SoundContext.h"

void main()
{
	try
	{
		SoundDevice device;
		SoundContext context(&device);
		context.SetCurrent();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception: " << ex.what() << std::endl;
	}
}