#version 430 core

uniform samplerCube cubeSampler;

in vec3 vertexPositionB;

void main()
{
	vec3 color = texture(cubeSampler, vertexPositionB).rgb;
	gl_FragColor = vec4(color, 1.0);
}
