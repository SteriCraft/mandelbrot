#pragma once

#include "sharedData.h"

struct Subwindow
{
	unsigned int xMin;
	unsigned int xMax;

	unsigned int yMin;
	unsigned int yMax;
};

class Plot
{
	public:
		Plot(SharedData *data);

		void compute();
		void computeWithThreads();
		void handleResize();

	private:
		void threadComputation(Subwindow limits);
		void initData();
		void setPixelColor(unsigned int x, unsigned int y, sf::Color color);
		void updatePlotSettings();
		void resetPixels();

		sf::Vector2f screenToWorld(sf::Vector2i pos);
		sf::Vector2f screenToWorld(int x, int y);
		sf::Vector2i worldToScreen(sf::Vector2f pos);
		sf::Vector2i worldToScreen(float x, float y);

		static float lerp(float rangeMin, float rangeMax, float x);

		SharedData *m_data;
		Bounds m_plotBounds;
		unsigned int m_windowWidth;
		unsigned int m_windowHeight;

		std::vector<std::thread*> m_threads;

		sf::Clock m_chrono;
		sf::Time m_timePoint;
};
