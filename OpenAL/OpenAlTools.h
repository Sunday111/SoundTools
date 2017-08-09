#pragma once

#include <stdexcept>

#include <AL/al.h>
#include <AL/alc.h>

static constexpr auto alInvalidId = std::numeric_limits<ALuint>::max();

template<typename Ret, typename... Args>
Ret OpenAlCall(Ret(*fn)(Args...), Args... args)
{
	auto ret = fn(args...);
	auto error = alGetError();

	if (error != AL_NO_ERROR)
	{
		auto message = alGetString(error);
		throw std::runtime_error(message);
	}

	return ret;
}

template<typename... Args>
void OpenAlCallVoid(void(*fn)(Args...), Args... args)
{
	fn(args...);
	auto error = alGetError();

	if (error != AL_NO_ERROR)
	{
		auto message = alGetString(error);
		throw std::runtime_error(message);
	}
}