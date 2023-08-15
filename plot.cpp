#include <iostream>
#include <complex>

#include "plot.h"
#include "mandelbrot.h"

Plot::Plot(SharedData *data)
{
	m_data = data;

	initData();
}

void Plot::compute()
{
    sf::Vector2f coord;
    Mandelbrot::Result endPoint;
    int computations(0);

    updatePlotSettings();
    m_data->eventType = Event::NONE;

    m_timePoint = m_chrono.getElapsedTime();
    for (unsigned int x(0); x < m_windowWidth; x++)
    {
        for (unsigned int y(0); y < m_windowHeight; y++)
        {
            if (m_data->eventType == Event::CAMERA_MOVED || m_data->eventType == Event::WINDOW_RESIZED)
                return;

            coord = screenToWorld(x, y);

            endPoint = Mandelbrot::computeMandelbrot(coord.x, coord.y, m_data->maxIterations);
            setPixelColor(x, y, Mandelbrot::getColor(endPoint, m_data->maxIterations));

            computations++;
            m_data->progress = (float)computations / (m_windowWidth * m_windowHeight) * 100.f;
        }
    }

    m_data->computationTime = ((float)m_chrono.getElapsedTime().asMicroseconds() - (float)m_timePoint.asMicroseconds()) / 1000.f; // µs
    m_data->progress = 100.f;
    m_data->eventType = Event::NONE;
}

void Plot::computeWithThreads()
{
    unsigned int maxThreads(std::thread::hardware_concurrency());
    std::vector<Subwindow> screenDivisionsBounds;

    updatePlotSettings();
    m_data->eventType = Event::NONE;

    Subwindow limits;
    for (unsigned int i(0); i < maxThreads; i++)
    {
        limits.xMin = (i % (maxThreads / 2)) * (2 * m_windowWidth / maxThreads);

        if (i == maxThreads / 2 - 1 || i == maxThreads - 1)
        {
            limits.xMax = m_windowWidth - 1;
        }
        else
        {
            limits.xMax = ((i % (maxThreads / 2)) + 1) * (2 * m_windowWidth / maxThreads);
        }

        if (i / (maxThreads / 2))
        {
            limits.yMin = m_windowHeight / 2;
            limits.yMax = m_windowHeight - 1;
        }
        else
        {
            limits.yMin = 0;
            limits.yMax = m_windowHeight / 2 - 1;
        }

        screenDivisionsBounds.push_back(limits);
    }

    // Create a thread for each division with limits as parameters
    // Cancel everything if the camera moves again

    m_timePoint = m_chrono.getElapsedTime();
    m_data->nbWorkerComplete = 0;
    m_data->computations = 0;
    m_threads.clear();
    m_data->stop = false;
    for (unsigned int i(0); i < maxThreads; i++)
    {
        // Create the thread and give it the bounds
        m_threads.push_back(new std::thread(&Plot::threadComputation, this, screenDivisionsBounds[i]));
    }

    while (m_data->nbWorkerComplete < maxThreads)
    {
        if (m_data->eventType == Event::CAMERA_MOVED || m_data->eventType == Event::WINDOW_RESIZED)
        {
            m_data->stop;
            return;
        }

        m_data->progress = (float)m_data->computations / (m_windowWidth * m_windowHeight) * 100.f;
    }

    m_data->computationTime = ((float)m_chrono.getElapsedTime().asMicroseconds() - (float)m_timePoint.asMicroseconds()) / 1000.f; // µs
    m_data->progress = 100.f;
    m_data->eventType = Event::NONE;
}

void Plot::threadComputation(Subwindow limits)
{
    sf::Vector2f coord;
    Mandelbrot::Result endPoint;

    std::cout << "Thread limits: xMin: " << limits.xMin << " ; xMax: " << limits.xMax << std::endl;

    for (unsigned int x(limits.xMin); x < limits.xMax; x++)
    {
        for (unsigned int y(limits.yMin); y < limits.yMax; y++)
        {
            if (m_data->stop)
                return;

            coord = screenToWorld(x, y);

            endPoint = Mandelbrot::computeMandelbrot(coord.x, coord.y, m_data->maxIterations);
            setPixelColor(x, y, Mandelbrot::getColor(endPoint, m_data->maxIterations));

            m_data->mutex.lock();
            m_data->computations++;
            m_data->mutex.unlock();
        }
    }

    m_data->mutex.lock();
    m_data->nbWorkerComplete++;
    m_data->mutex.unlock();
}

void Plot::handleResize()
{
    updatePlotSettings();
    resetPixels();
}

void Plot::initData()
{
    m_data->pixels = new sf::Uint8[DEFAULT_WIN_WIDTH * DEFAULT_WIN_HEIGHT * 4];

    int index(0);
    for (unsigned int x(0); x < DEFAULT_WIN_WIDTH; x++)
    {
        for (unsigned int y(0); y < DEFAULT_WIN_HEIGHT; y++)
        {
            index = (x + y * DEFAULT_WIN_WIDTH) * 4;

            m_data->pixels[index]     = 10;
            m_data->pixels[index + 1] = 10;
            m_data->pixels[index + 2] = 10;
            m_data->pixels[index + 3] = 255;
        }
    }
}

void Plot::setPixelColor(unsigned int x, unsigned int y, sf::Color color)
{
    m_data->mutex.lock();

    int index = (x + y * m_windowWidth) * 4;

    m_data->pixels[index]     = color.r;
    m_data->pixels[index + 1] = color.g;
    m_data->pixels[index + 2] = color.b;
    m_data->pixels[index + 3] = color.a;

    m_data->mutex.unlock();
}

void Plot::updatePlotSettings()
{
    m_data->mutex.lock();

    m_plotBounds = m_data->plotBounds;
    m_windowWidth = m_data->windowWidth;
    m_windowHeight = m_data->windowHeight;

    m_data->mutex.unlock();
    m_data->eventType = Event::CAMERA_MOVED;
}

void Plot::resetPixels()
{
    m_data->mutex.lock();

    delete m_data->pixels;
    m_data->pixels = new sf::Uint8[m_data->windowWidth * m_data->windowHeight * 4];

    m_data->resizeEventProcessed = true;

    m_data->mutex.unlock();
}

sf::Vector2f Plot::screenToWorld(sf::Vector2i pos)
{
    return screenToWorld(pos.x, pos.y);
}

sf::Vector2f Plot::screenToWorld(int x, int y)
{
    sf::Vector2f coord;
    float xSpan(m_plotBounds.xMax - m_plotBounds.xMin);
    float ySpan(m_plotBounds.yMax - m_plotBounds.yMin);

    coord.x = lerp(0.f, (float)m_windowWidth, (float)x) * xSpan + m_plotBounds.xMin;
    coord.y = lerp(0.f, (float)m_windowHeight, (float)y) * ySpan + m_plotBounds.yMin;

    return coord;
}

sf::Vector2i Plot::worldToScreen(sf::Vector2f pos)
{
    return worldToScreen(pos.x, pos.y);
}

sf::Vector2i Plot::worldToScreen(float x, float y)
{
    sf::Vector2i pos;

    pos.x = (int)(lerp(m_plotBounds.xMin, m_plotBounds.xMax, x) * m_windowWidth);
    pos.y = (int)(lerp(m_plotBounds.yMin, m_plotBounds.yMax, y) * m_windowHeight);

    return pos;
}

float Plot::lerp(float rangeMin, float rangeMax, float x)
{
    return (x - rangeMin) / (rangeMax - rangeMin);
}
