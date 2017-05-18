#include "Graphics.hpp"

#include <stdexcept>
#include <vector>
#include <cmath>
#include "Common.hpp"
#include <iostream>

constexpr unsigned int CURVE_SEGMENTS = 64;

GLuint compileShader(const GLenum type, const std::string& source)
{	
	GLuint shader = glCreateShader(type);
	auto cString = source.c_str();
	glShaderSource(shader, 1, &cString, NULL);
	glCompileShader(shader);
	
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
		throw std::runtime_error(std::string("Error compiling shader: ") + infoLog);
	}
	return shader;
}

bool Vector::in(Vector cornerA, Vector cornerB)
{
	return x > std::min(cornerA.x, cornerB.x) &&
	       x < std::max(cornerA.x, cornerB.x) &&
	       y > std::min(cornerA.y, cornerB.y) &&
	       y < std::max(cornerA.y, cornerB.y);
}


Vector Vector::fromPolar(float angle, float radius)
{
	return {std::cos(angle)*radius, std::sin(angle)*radius};
}

Vector operator+(const Vector left, const Vector right)
{
	return {left.x+right.x, left.y+right.y};
}

Vector operator-(const Vector left, const Vector right)
{
	return {left.x-right.x, left.y-right.y};
}

Vector operator*(const Vector vector, const float scalar)
{
	return {vector.x*scalar, vector.y*scalar};
}

Vector operator*(const float scalar, const Vector vector)
{
	return vector*scalar;
}

Vector operator/(const Vector vector, const float scalar)
{
	return {vector.x/scalar, vector.y/scalar};
}

namespace
{
	void draw(const ShaderProgram& shader, const Color color, const GLenum mode, const std::vector<Vector>& vertices)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		std::vector<GLfloat> flattenedVertices;
		flattenedVertices.reserve(vertices.size()*2);
		for (const Vector& vertex: vertices)
		{
			flattenedVertices.push_back(vertex.x);
			flattenedVertices.push_back(vertex.y);
		}
		
		glBufferData(GL_ARRAY_BUFFER, flattenedVertices.size()*sizeof(GLfloat), flattenedVertices.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		
		glUniform3f(shader.colorUniformLocation, color.red, color.green, color.blue);
		
		glDrawArrays(mode, 0, vertices.size());
		
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
}

void drawPie(const ShaderProgram& shader,
             const Color color,
             const Vector position,
             const float radius,
             float begin,
             float length)
{
	const float step = length/CURVE_SEGMENTS;
	std::vector<Vector> vertices = {position};
	for (unsigned int triangle = 0; triangle < CURVE_SEGMENTS; triangle++)
	{
		const float angle1 = begin + step*triangle;
		const float angle2 = begin + step*(triangle+1);
		vertices.push_back(position + Vector{std::cos(angle1), std::sin(angle1)} * radius);
		vertices.push_back(position + Vector{std::cos(angle2), std::sin(angle2)} * radius);
	}
	draw(shader, color, GL_TRIANGLE_FAN, vertices);
}

void drawLine(const ShaderProgram& shader,
              const Color color,
              const Vector from,
              const Vector to,
              const float width)
{
	const float angle = std::atan2((to-from).y, (to-from).x);
	const Vector offset = Vector{std::cos(angle+PI/2), std::sin(angle+PI/2)}*width/2;
	const Vector corners[] = {to+offset, from+offset, from-offset, to-offset};
	const std::vector<Vector> vertices = {
		corners[0], corners[1], corners[2],
		corners[2], corners[3], corners[0]
	};
	draw(shader, color, GL_TRIANGLES, vertices);
}

void drawCappedLine(const ShaderProgram& shader,
                    const Color color,
                    const Vector from,
                    const Vector to,
                    const float width)
{
	drawLine(shader, color, from, to, width);
	const float angle1 = std::atan2((to-from).y, (to-from).x) + PI/2;
	const float angle2 = angle1+PI;
	drawPie(shader, color, from, width/2, angle1, PI);
	drawPie(shader, color, to, width/2, angle2, PI);
}

void drawArc(const ShaderProgram& shader,
             Color color,
             Vector position,
             float radius,
             float begin,
             float length,
             float width)
{
	std::vector<Vector> vertices;
	vertices.reserve((CURVE_SEGMENTS+1)*2);
	vertices.push_back(position+Vector::fromPolar(begin, radius-width/2));
	vertices.push_back(position+Vector::fromPolar(begin, radius+width/2));
	for (unsigned int segment = 0; segment <= CURVE_SEGMENTS; segment++)
	{
		const float angle = begin + (length/CURVE_SEGMENTS)*segment;
		vertices.push_back(position+ Vector::fromPolar(angle, radius-width/2));
		vertices.push_back(position+ Vector::fromPolar(angle, radius+width/2));
	}
	draw(shader, color, GL_TRIANGLE_STRIP, vertices);
}

void drawRectangle(const ShaderProgram& shader,
                   Color color,
                   Vector cornerA,
                   Vector cornerB)
{
	std::vector<Vector> vertices = {
		cornerA, {cornerB.x, cornerA.y}, cornerB,
		cornerB, {cornerA.x, cornerB.y}, cornerA
	};
	draw(shader, color, GL_TRIANGLES, vertices);
}

void drawX(const ShaderProgram& shader,
           Color color,
           Vector position)
{
	const Vector offset1 = Vector{1, 1}/8*0.8;
	const Vector offset2 = {offset1.x, -offset1.y};
	drawCappedLine(shader, color, position+offset1, position-offset1, 0.05);
	drawCappedLine(shader, color, position+offset2, position-offset2, 0.05);
}

void drawO(const ShaderProgram& shader,
           Color color,
           Vector position)
{
	drawArc(shader, color, position, (1.0/8)*0.8, 0, PI*2, 0.05);
}

void drawSymbol(const ShaderProgram& shader,
                Color color,
                Symbol symbol,
                Vector position)
{
	if (symbol == Symbol::X) drawX(shader, color, position);
	else if (symbol == Symbol::O) drawO(shader, color, position);
}

void drawGrid(const ShaderProgram& shader,
              Color color)
{
	const float INNER_SIZE = 0.9;
	const float width = 0.05;
	
	drawCappedLine(shader, color, {-INNER_SIZE, -INNER_SIZE/2}, {INNER_SIZE, -INNER_SIZE/2}, width);
	drawCappedLine(shader, color, {-INNER_SIZE, 0}, {INNER_SIZE, 0}, width);
	drawCappedLine(shader, color, {-INNER_SIZE, INNER_SIZE/2}, {INNER_SIZE, INNER_SIZE/2}, width);
	
	drawCappedLine(shader, color, {-INNER_SIZE/2, -INNER_SIZE}, {-INNER_SIZE/2, INNER_SIZE}, width);
	drawCappedLine(shader, color, {0, -INNER_SIZE}, {0, INNER_SIZE}, width);
	drawCappedLine(shader, color, {INNER_SIZE/2, -INNER_SIZE}, {INNER_SIZE/2, INNER_SIZE}, width);
}

void drawGame(ShaderProgram& shaderProgram, const GameState& gameState, const Color gridColor)
{
	drawGrid(shaderProgram, gridColor);
	for (unsigned int row = 0; row < 4; row++)
	{
		for (unsigned int column = 0; column < 4; column++)
		{
			const unsigned int place = row*4+column;
			drawSymbol(shaderProgram, WHITE, gameState.symbols[place], spaceCenter(row, column));
		}
	}
}

Vector spaceCenter(unsigned int row, unsigned int column)
{
	return Vector{column/2.0f-1+0.25f, row/-2.0f+1-0.25f}*INNER_SIZE;
}
