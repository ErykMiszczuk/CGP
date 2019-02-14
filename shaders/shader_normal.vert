#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightDir;
uniform vec3 cameraPos;

out vec3 interpNormal;
out vec3 interpPos;
out vec2 interpTexCoord;

out vec3 lightWorld;
out vec3 cameraWorld;
out vec3 interpPosWorld;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	
	interpTexCoord = vec2(vertexTexCoord.x, vertexTexCoord.y);
	interpNormal = normalize(modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	interpPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
	
	vec3 interpTangent = normalize(modelMatrix * vec4(vertexTangent, 0.0)).xyz;
	vec3 interpBitangent = normalize(cross(interpNormal, interpTangent));
	
	lightWorld.x = dot(lightDir, interpTangent);
	lightWorld.y = dot(lightDir, interpBitangent);
	lightWorld.z = dot(lightDir, interpNormal);
	lightWorld = normalize(lightWorld);
	
	cameraWorld.x = dot(cameraPos, interpTangent);
	cameraWorld.y = dot(cameraPos, interpBitangent);
	cameraWorld.z = dot(cameraPos, interpNormal);
		
	interpPosWorld.x = dot(interpPos, interpTangent);
	interpPosWorld.y = dot(interpPos, interpBitangent);
	interpPosWorld.z = dot(interpPos, interpNormal);
	interpPosWorld = normalize(interpPosWorld);
}
