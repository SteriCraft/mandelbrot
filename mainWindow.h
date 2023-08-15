#pragma once

#include <SFML/Graphics.hpp>
#include <thread>

#include "sharedData.h"

#define DEBUG_SPACING 5

#define DEFAULT_ZOOM 0.003f
#define DEFAULT_CAMERA_POSITION sf::Vector2f(-0.5f, 0.f)

namespace Display
{	
	enum class State { CLOSED = 0, OPENED };
	enum class Command { NONE = 0, CLOSE };

	struct ThreadControl
	{
		State currentState = State::CLOSED;
		Command cmd = Command::NONE;

		std::mutex mutex;
	};

	struct DebugPanel
	{
		sf::Font *font;

		sf::Text *fpsTxt;
		sf::Text *mousePlotPositionTxt;
		sf::Text *plotBoundsTxt;
		sf::Text *cameraPosTxt;
		sf::Text *zoomTxt;
		sf::Text *maxIterationsTxt;
		sf::Text* threadingTxt;
		sf::Text *helpTxt;
		sf::Text *progressTxt;
	};

	enum class LineType { ABSOLUTE_MAIN = 0, MAIN, NORMAL };

	struct Line
	{
		LineType type = LineType::NORMAL;

		int pos = 0;
		float plotPos = 0;

		sf::Text lightnessueLabelTxt;
	};

	struct Grid
	{
		bool show = true;

		std::vector<Line> m_abscissLines;
		std::vector<Line> m_ordinateLines;
	};

	class MainWindow {
		public:
			MainWindow(SharedData *data);
			~MainWindow();

			bool isOpen();

		private:
			void windowLoop();
			bool timeForNextFrame();

			void handleEvents();
			void checkCommands();
			void update();
			void updateScreenBuffer();
			void computeGrid();
			void createGridLabels();
			void drawGrid();

			void initDebugPanel();
			void drawDebugInfo();
			void deleteDebugPointers();

			void getBounds();

			sf::View *m_dynamicView;
			sf::View *m_staticView;
			sf::Vector2f m_cameraPosition;
			float m_zoom;
			bool m_moving;
			sf::Vector2i m_lastMousePosition;
			Grid m_grid;

			sf::RenderWindow *m_window;
			sf::Image m_screenImage;
			sf::Texture m_screenTexture;
			sf::Sprite m_screenSprite;
			SharedData *m_data;

			sf::Clock m_chrono;
			sf::Time m_timePoint;
			float m_framerate;

			bool m_showDebug;
			DebugPanel m_debugPanel;

			ThreadControl m_status;
			std::thread	*m_windowThread;

			static std::string decimal2str(float value, unsigned int precision = 2);
	};
}
