#pragma once


class FrameTime {
public:
	FrameTime(int maxFPS);

	void startNewFrame();
	float getDeltaTime();

private:
	const float minFrameDuration;
	float lastTime;
	float deltaTime;
};