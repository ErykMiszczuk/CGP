#version 430 core

uniform sampler2D textureColor;
uniform sampler2D textureNormal;
uniform sampler2D depthMap;
//materials texture
uniform sampler2D textureMetallic;
uniform sampler2D textureRoughness;

uniform vec3 objectColor;
uniform vec3 lightDir;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 interpPos;
in vec2 interpTexCoord;

in vec3 lightWorld;
in vec3 cameraWorld;
in vec3 interpPosWorld;

in vec4 lightPosition;

// material parameters
vec3  albedo;
float metallic;
float roughness;
float ao;

const float PI = 3.14159265359;

float calculateShadow(vec4 lightPosition, vec3 normal)
{
	vec3 divided = lightPosition.xyz / lightPosition.w;
	divided = divided * 0.5 + 0.5;
	
	float closestDepth = texture2D(depthMap, divided.xy).r;
	float currentDepth = divided.z;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	//float shadow = currentDepth > closestDepth + bias  ? 1.0 : 0.0;
	
	// new
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, divided.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	//
	
	if(currentDepth > 1.0)
        shadow = 0.0;

	return shadow;
}

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y);
	vec3 color = texture2D(textureColor, modifiedTexCoord).rgb;
	
	vec3 normal = texture2D(textureNormal, modifiedTexCoord).rgb;
	normal = (normalize(normal) - 0.5f) * 2.0f;

	float diffuse = max(dot(normal, -lightWorld), 0.0);
	
	vec3 toEye = normalize(cameraPos - interpPos);
	float specular = pow(max(dot(toEye, reflect(lightDir, normal)), 0.0), 30.0);
	
	float shadow = calculateShadow(lightPosition, normal);
	
	gl_FragColor = vec4((color - shadow) * diffuse, 1.0) + vec4(vec3(1.0) * specular, 0.0);
}