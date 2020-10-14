#pragma once
#include <vector>
#include <functional>

class PoolReturnCallback
{
public:
	void AddCallback(std::function<void()> callback)
	{
		this->callbacks.push_back(callback);
	}

	void OnReturn()
	{
		for (auto i : callbacks) i();
	}

private:
	std::vector<std::function<void()>> callbacks;
};

