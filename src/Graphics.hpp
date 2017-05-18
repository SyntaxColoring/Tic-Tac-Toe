#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

#include <string>
#include "GLEW.hpp"
#include "Shader.hpp"
#include "Game.hpp"

// We don't draw anything in a small border along the edges of the window.
// This defines the size of the area we do draw in.
constexpr float INNER_SIZE = 0.9;

struct Color
{
	float red;
	float green;
	float blue;
};
constexpr Color DARK_GRAY  = {  0.0/255,  43.0/255,  54.0/255};
constexpr Color LIGHT_GRAY = {  7.0/255,  54.0/255,  66.0/255};
constexpr Color WHITE      = {250.0/255, 250.0/255, 250.0/255};
constexpr Color RED        = {220.0/255,  50.0/255,  47.0/255};
constexpr Color YELLOW     = {181.0/255, 137.0/255,   0.0/255};
constexpr Color GREEN      = {133.0/255, 153.0/255,   0.0/255};

struct Vector
{
	float x;
	float y;
	static Vector fromPolar(float angle, float radius);
	bool in(Vector cornerA, Vector cornerB);
};
Vector operator+(Vector, Vector);
Vector operator-(Vector, Vector);
Vector operator*(Vector, float);
Vector operator*(float, Vector);
Vector operator/(Vector, float);

GLuint compileShader(GLenum type, const std::string& source);

void drawPie(const ShaderProgram& shader,
             Color color,
             Vector position,
             float radius,
             float begin,
             float length);

void drawLine(const ShaderProgram& shader,
              Color color,
              Vector from,
              Vector to,
              float width);

void drawCappedLine(const ShaderProgram& shader,
                    Color color,
                    Vector from,
                    Vector to,
                    float width);

void drawArc(const ShaderProgram& shader,
             Color color,
             Vector position,
             float radius,
             float begin,
             float length,
             float width);

void drawRectangle(const ShaderProgram& shader,
                   Color color,
                   Vector cornerA,
                   Vector cornerB);

void drawX(const ShaderProgram& shader,
           Color color,
           Vector position);

void drawO(const ShaderProgram& shader,
           Color color,
           Vector position);

void drawSymbol(const ShaderProgram& shader,
                Color color,
                Symbol symbol,
                Vector position);

void drawGrid(const ShaderProgram& shader,
              Color color);

void drawGame(ShaderProgram& shaderProgram, const GameState& gameState, const Color gridColor);

// Returns the normalized device coordinates of the center of the space
// indexed by row and column.
Vector spaceCenter(unsigned int row, unsigned int column);
#endif
