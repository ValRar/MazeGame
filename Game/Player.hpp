#pragma once
#include <chrono>
template <typename T>
struct Point {
	T x, y;
	Point() : x{ 0 }, y{ 0 } {}
	Point(T x, T y) : x{ x }, y{ y } {}
};

class Player {
private:
	const std::chrono::milliseconds animationDuration{100};
	int targetX, targetY;
	int currentX, currentY;
	std::chrono::milliseconds animationStartTime{-1};

	double getAnimationProgress() {
		return ((std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()) - animationStartTime) * 100 / animationDuration) / static_cast<double>(100);
	}
public:
	Player(int startX, int startY) : targetX{ startX }, targetY{ startY }
	{
		currentX = targetX;
		currentY = targetY;
	}
	bool IsInAnimation() {
		return animationStartTime != std::chrono::milliseconds{ -1 } &&
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now().time_since_epoch()) - animationStartTime < animationDuration;
	}
	void Move(int dx, int dy) {
		targetX += dx;
		targetY += dy;
		if (!IsInAnimation())
			animationStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now().time_since_epoch());
	}
	double GetCurrentX() {
		if (IsInAnimation())
			return (double)currentX + (double)(targetX - currentX) * getAnimationProgress();
		else {
			currentX = targetX;
			return currentX;
		}
	}
	double GetCurrentY() {
		if (IsInAnimation())
			return (double)currentY + (double)(targetY - currentY) * getAnimationProgress();
		else {
			currentY = targetY;
			return currentY;
		}
	}
	double GetTargetX() {
		return targetX;
	}
	double GetTargetY() {
		return targetY;
	}
	Point<int> GetTargetPoint() {
		return std::move(Point<int>(targetX, targetY));
	}
	Point<double> GetCurrentPoint() {
		return std::move(Point<double>(GetCurrentX(), GetCurrentY()));
	}
};