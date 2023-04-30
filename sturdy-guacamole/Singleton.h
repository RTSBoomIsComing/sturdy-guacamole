#pragma once
#include <stdexcept>

template <typename T>
class Singleton
{
protected:
	inline static T* s_instance{};

	Singleton()
	{
		if (s_instance)
			throw std::logic_error("Singleton already exists.");

		s_instance = reinterpret_cast<T*>(this);
	}

	~Singleton()
	{
		s_instance = nullptr;
	}

public:
	// Inherit with protected when you want to hide GetInstance()
	static T& Get()
	{
		if (!s_instance)
			throw std::logic_error("Singleton does not exist.");

		return *s_instance;
	}
};
