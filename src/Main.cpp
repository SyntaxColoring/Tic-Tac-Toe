#include <stdexcept>
#include <SDL.h>
#include "GLEW.hpp"
#include "Shader.hpp"
#include "Graphics.hpp"
#include "Game.hpp"
#include "AI.hpp"
#include "Common.hpp"

#include <vector>
#include <future>
#include <chrono>
#include <iostream>

namespace
{
	// Sets the OpenGL viewport to match the window size.
	void setViewport(SDL_Window* const window)
	{
		int viewportWidth, viewportHeight;
		SDL_GL_GetDrawableSize(window, &viewportWidth, &viewportHeight);
		glViewport(0, 0, viewportWidth, viewportHeight);
	}

	// Returns the mouse position in normalized device coordinates.
	Vector getMousePosition(SDL_Window* const window)
	{
		int x, y;
		int width, height;
		SDL_GetMouseState(&x, &y);
		SDL_GL_GetDrawableSize(window, &width, &height);
		return {(float)x/width*2-1, (float)y/height*-2+1};
	}

	enum class State
	{
		DIFFICULTY_SELECTION,
		SYMBOL_SELECTION,
		GAMEPLAY_PLAYER_TURN,
		GAMEPLAY_AI_TURN_BEGIN,
		GAMEPLAY_AI_TURN_WAITING,
		GAME_OVER
	};
}

int main()
{	
	if (SDL_Init(SDL_INIT_VIDEO))
		throw std::runtime_error(std::string("Error initializing SDL: ") + SDL_GetError());
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	
	SDL_Window* window = SDL_CreateWindow("Tic-Tac-Toe",
	                                      SDL_WINDOWPOS_CENTERED,
	                                      SDL_WINDOWPOS_CENTERED,
	                                      480,
	                                      480,
	                                      SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	
	if (!window)
		throw std::runtime_error(std::string("Error creating window: ") + SDL_GetError());
	
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
		throw std::runtime_error(std::string("Error creating context: ") + SDL_GetError());
	
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Error initializing GLEW.");
	
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
	
	ShaderProgram shaderProgram;
	shaderProgram.id = glCreateProgram();
	glAttachShader(shaderProgram.id, vertexShader);
	glAttachShader(shaderProgram.id, fragmentShader);
	glLinkProgram(shaderProgram.id);
	glUseProgram(shaderProgram.id);
	shaderProgram.vertexAttributeLocation = 0;
	shaderProgram.colorUniformLocation = glGetUniformLocation(shaderProgram.id, "inColor");
		
	glClearColor(DARK_GRAY.red, DARK_GRAY.green, DARK_GRAY.blue, 1.0f);	
	setViewport(window);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe.
	
	auto state = State::DIFFICULTY_SELECTION;
	unsigned int difficultyLevel = 0;
	Symbol playerSymbol = Symbol::EMPTY;
	GameState gameState;
	
	std::future<Action> aiDecision;
		
	bool done = false;
	while (!done)
	{
		bool mouseReleased = false;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_WINDOWEVENT:
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							setViewport(window);
							break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button == 1) mouseReleased = true;
					break;
				case SDL_QUIT:
					done = true;
					break;
			}
		}
		
		Vector mouse = getMousePosition(window);
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		if (state == State::DIFFICULTY_SELECTION)
		{
			std::vector<std::pair<Vector, Vector>> rectangles = {
				{{-0.5, 0.125}, {0.5, 0.375}},
				{{-0.5, -0.125}, {0.5, 0.125}},
				{{-0.5, -0.375}, {0.5, -0.125}}
			};
			
			for (std::size_t index = 0; index < rectangles.size(); index++)
			{
				const auto& rectangle = rectangles[index];
				if (mouse.in(rectangle.first, rectangle.second))
				{
					drawRectangle(shaderProgram, LIGHT_GRAY, rectangle.first, rectangle.second);
					if (mouseReleased)
					{
						difficultyLevel = index;
						state = State::SYMBOL_SELECTION;
						break;
					}
				}
			}
			
			// Draw difficulty selection bars.
			drawPie(shaderProgram, GREEN, {-0.25, 0.25}, 0.0625, 0.0, PI*2);
			drawPie(shaderProgram, YELLOW, {-0.25, 0.0}, 0.0625, 0.0, PI*2);
			drawPie(shaderProgram, YELLOW, {0.0, 0.0}, 0.0625, 0.0, PI*2);
			drawPie(shaderProgram, RED, {-0.25, -0.25}, 0.0625, 0.0, PI*2);
			drawPie(shaderProgram, RED, {0.0, -0.25}, 0.0625, 0.0, PI*2);
			drawPie(shaderProgram, RED, {0.25, -0.25}, 0.0625, 0.0, PI*2);
		}
		
		else if (state == State::SYMBOL_SELECTION)
		{
			if (mouse.in({-0.75, -0.25}, {-0.25, 0.25})) // Over left (X) side.
			{
				drawRectangle(shaderProgram, LIGHT_GRAY, {-0.75, -0.25}, {-0.25, 0.25});
				if (mouseReleased)
				{
					playerSymbol = Symbol::X;
					state = State::GAMEPLAY_PLAYER_TURN;
				}
			}
			
			if (mouse.in({0.75, 0.25}, {0.25, -0.25})) // Over right (O) side.
			{
				drawRectangle(shaderProgram, LIGHT_GRAY, {0.75, 0.25}, {0.25, -0.25});
				if (mouseReleased)
				{
					playerSymbol = Symbol::O;
					state = State::GAMEPLAY_AI_TURN_BEGIN;
				}
			}
			
			drawX(shaderProgram, WHITE, {-0.5, 0});
			drawCappedLine(shaderProgram, WHITE, {0, -0.5}, {0, 0.5}, 0.05);
			drawO(shaderProgram, WHITE, {0.5, 0});
		}
		
		else if (state == State::GAMEPLAY_PLAYER_TURN || state == State::GAMEPLAY_AI_TURN_BEGIN || state == State::GAMEPLAY_AI_TURN_WAITING)
		{
			if (state == State::GAMEPLAY_PLAYER_TURN)
			{
				unsigned int row = (unsigned int)((-mouse.y/INNER_SIZE+1)*2);
				unsigned int column = (unsigned int)((mouse.x/INNER_SIZE+1)*2);
				if (row > 3) row = 3;
				if (column > 3) column = 3;
				const unsigned int place = row*4+column;
				
				if (gameState.symbols[place] == Symbol::EMPTY)
				{
					drawSymbol(shaderProgram, YELLOW, playerSymbol, spaceCenter(row, column));
					
					if (mouseReleased)
					{
						Action action = {playerSymbol, place};
						gameState = gameState.apply(action);
						if (gameState.terminal()) state = State::GAME_OVER;
						else state = State::GAMEPLAY_AI_TURN_BEGIN;
					}
				}
			}
			
			else if (state == State::GAMEPLAY_AI_TURN_BEGIN)
			{
				// Set up the AI's turn.  We'll wait while it thinks in another thread.
				const unsigned int maximumDepths[] = {0, 1, 6};
				aiDecision = std::async(std::launch::async, findBestAction, gameState, improvedEvaluator, opponentOf(playerSymbol), maximumDepths[difficultyLevel]);
				state = State::GAMEPLAY_AI_TURN_WAITING;
			}
			
			else if (state == State::GAMEPLAY_AI_TURN_WAITING)
			{
				if (aiDecision.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
				{
					gameState = gameState.apply(aiDecision.get());
					if (gameState.terminal()) state = State::GAME_OVER;
					else state = State::GAMEPLAY_PLAYER_TURN;
				}
			}
			
			drawGame(shaderProgram, gameState, WHITE);	
		}
		
		else if (state == State::GAME_OVER)
		{
			Color gridColor = YELLOW;
			if (gameState.winner() == playerSymbol) gridColor = GREEN;
			else if (gameState.winner() == opponentOf(playerSymbol)) gridColor = RED;
			drawGame(shaderProgram, gameState, gridColor);
			
			if (mouseReleased)
			{
				gameState = GameState();
				state = State::DIFFICULTY_SELECTION;
			}
		}
		
		SDL_GL_SwapWindow(window);
	}
}
