#include "mainWindow.h"
#include "plot.h"

int main()
{
    SharedData data;

    Plot plot(&data);
    Display::MainWindow* win = new Display::MainWindow(&data);

    while (win->isOpen())
    {
        if (data.eventType == Event::CAMERA_MOVED)
        {
            if (data.useThreading)
                plot.computeWithThreads();
            else
                plot.compute();
        }
        else if (data.eventType == Event::WINDOW_RESIZED)
        {
            plot.handleResize();
        }
        else
        {
            sf::sleep(sf::milliseconds((int)(1000.f / FPS_TARGET)));
        }
    }

    delete win;

    return 0;
}
