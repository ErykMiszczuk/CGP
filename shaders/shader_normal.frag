#version 430 core

uniform sampler2D textureColor;
uniform sampler2D textureNormal;

uniform vec3 objectColor;
uniform vec3 lightDir;

in vec3 interpNormal;
in vec3 interpPos;
in vec2 interpTexCoord;

in vec3 lightWorld;
in vec3 cameraWorld;
in vec3 interpPosWorld;


void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y);
	vec3 color = texture2D(textureColor, modifiedTexCoord).rgb;
	vec3 normal = texture2D(textureNormal, modifiedTexCoord).rgb;
	normal = (normalize(normal) - 0.5f) * 2.0f;

	
	
	float diffuse = max(dot(normal, -lightWorld), 0.0);
	
	vec3 toEye = normalize(cameraWorld - interpPosWorld);
	float specular = pow(max(dot(toEye, reflect(lightWorld, normal)), 0.0), 30.0);
	
	gl_FragColor = vec4(color * diffuse, 1.0) + vec4(vec3(1.0) * specular, 0.0);
}
