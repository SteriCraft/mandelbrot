#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>

#define FPS_TARGET 60.f
#define DEBUG_FONT_SIZE 16

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

#define DEFAULT_MAX_ITERATIONS 256

enum class Event { NONE = 0, CAMERA_MOVED, WINDOW_RESIZED };

struct Bounds
{
	float xMin;
	float xMax;

	float yMin;
	float yMax;
};

struct SharedData
{
	sf::Uint8* pixels = nullptr;
	unsigned int windowWidth = DEFAULT_WIN_WIDTH;
	unsigned int windowHeight = DEFAULT_WIN_HEIGHT;

	bool resizeEventProcessed = false;
	Event eventType = Event::NONE;
	Bounds plotBounds = { 0.f, 0.f, 0.f, 0.f };

	float progress = 0.f;
	float computationTime = 0.f;
	int maxIterations = DEFAULT_MAX_ITERATIONS;

	bool useThreading = false;
	unsigned int nbWorkerComplete = 0;
	unsigned int computations = 0;
	bool stop = false;

	std::mutex mutex;
};
