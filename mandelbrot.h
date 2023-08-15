#pragma once

#include <complex>
#include <SFML/Graphics.hpp>

namespace Mandelbrot
{
	struct Result
	{
		std::complex<float> endPoint;
		int iterations = 0;
	};

	Result computeMandelbrot(float x, float y, unsigned int maxIterations);

	/*
	* Credits to "Tenry", member of the SFML-dev forum
	* https://stackoverflow.com/questions/369438/smooth-spectrum-for-mandelbrot-set-rendering
	*/
	sf::Color getColor(Result endPoint, int maxIterations);

	/*
	* Converts a given set of HSV float values into RGB coordinates.
	* The ouput is an sf::Color type from the SFML library.
	*
	* hue value is in range [0, 360], saturation and lightness in range [0, 1].
	*
	* Credits to fairlight1337
	* https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
	*/
	sf::Color HSVtoRGB(float hue, float saturation, float lightness);
}
