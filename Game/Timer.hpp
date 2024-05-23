#include <chrono>
#include <string>

class Timer
{
public:
	Timer() {
		startPoint = std::chrono::high_resolution_clock::now();
	}
	void Reset() {
		startPoint = std::chrono::high_resolution_clock::now();
	}
	std::chrono::milliseconds GetTimeMillis() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startPoint);
	}
	std::string GetFormattedString() {
		auto time = GetTimeMillis() / 10;
		auto millisString = std::to_string((time % 100).count());
		return std::to_string((time / 100).count()) + ":" + ((millisString.length() == 1) ? millisString + '0' : millisString);
	}
	~Timer() {}

private:
	std::chrono::steady_clock::time_point startPoint;
};
