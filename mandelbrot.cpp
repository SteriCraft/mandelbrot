#include <cmath>

#include "mandelbrot.h"

using namespace Mandelbrot;

Result Mandelbrot::computeMandelbrot(float x, float y, unsigned int maxIterations)
{
    Result end;
    std::complex<float> c(x, y);
    std::complex<float> z(0, 0);

    unsigned int n = 0;
    while (abs(z) < 2.0 && n < maxIterations)
    {
        z = (z * z) + c;
        n++;
    }

    end.endPoint = z;
    end.iterations = n;

    return end;
}

sf::Color Mandelbrot::getColor(Result endPoint, int maxIterations)
{
    if (endPoint.iterations == maxIterations)
    {
        return sf::Color::Black;
    }
    else
    {
        float smoothValue = endPoint.iterations + 1 - logf(logf(std::abs(endPoint.endPoint))) / logf(2.f);

        return HSVtoRGB(0.95f + 10 * smoothValue, 0.6f, 1.f);
    }
}

sf::Color Mandelbrot::HSVtoRGB(float hue, float saturation, float lightness)
{
    sf::Color result;

    float fR, fG, fB;
    float fC = lightness * saturation; // Chroma
    float huePrime = fmodf(hue / 60.f, 6);
    float fX = fC * (1 - fabs(fmodf(huePrime, 2.f) - 1));
    float fM = lightness - fC;

    if (0 <= huePrime && huePrime < 1)
    {
        fR = fC;
        fG = fX;
        fB = 0;
    }
    else if (1 <= huePrime && huePrime < 2)
    {
        fR = fX;
        fG = fC;
        fB = 0;
    }
    else if (2 <= huePrime && huePrime < 3)
    {
        fR = 0;
        fG = fC;
        fB = fX;
    }
    else if (3 <= huePrime && huePrime < 4)
    {
        fR = 0;
        fG = fX;
        fB = fC;
    }
    else if (4 <= huePrime && huePrime < 5)
    {
        fR = fX;
        fG = 0;
        fB = fC;
    }
    else if (5 <= huePrime && huePrime < 6)
    {
        fR = fC;
        fG = 0;
        fB = fX;
    }
    else {
        fR = 0;
        fG = 0;
        fB = 0;
    }

    fR += fM * 256;
    fG += fM * 256;
    fB += fM * 256;

    return sf::Color((sf::Uint8)(fR * 256), (sf::Uint8)(fG * 256), (sf::Uint8)(fB * 256));
}
