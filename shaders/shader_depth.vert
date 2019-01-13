#version 430 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 lightMVP;

void main()
{
	gl_Position = lightMVP * vec4(vertexPosition, 1.0);
}
