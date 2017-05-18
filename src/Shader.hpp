#ifndef SHADER_HPP_INCLUDED
#define SHADER_HPP_INCLUDED

constexpr auto VERTEX_SHADER_SOURCE = R"EOF(
#version 330 core
#extension all : disable
layout(location = 0) in vec2 position;
void main()
{
	gl_Position = vec4(position, 0, 1);
}
)EOF";

constexpr GLuint VERTEX_SHADER_POSITION_LOCATION = 0;

constexpr auto FRAGMENT_SHADER_SOURCE = R"EOF(
#version 330 core
#extension all : disable
layout(location = 0) out vec4 outColor;
uniform vec3 inColor;
void main()
{
	outColor = vec4(inColor, 1.0f);
}
)EOF";

struct ShaderProgram
{
	GLuint id;
	GLint vertexAttributeLocation;
	GLint colorUniformLocation;
};

#endif
