#version 430 core

uniform sampler2D textureSampler;
uniform sampler2D depthMap;

uniform vec3 lightDir;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 interpPos;
in vec2 interpTexCoord;

in vec4 lightPosition;

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

float fogFactorExp2(float dist, float density) {
  const float LOG2 = -1.442695;
  float d = density * dist;
  return 1.0 - clamp(exp2(d * d * LOG2), 0.0, 1.0);
}

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y);
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	
	vec3 normal = normalize(interpNormal);
	float diffuse = max(dot(normal, -lightDir), 0.0);
	
	vec3 toEye = normalize(cameraPos - interpPos);
	float specular = pow(max(dot(toEye, reflect(lightDir, normal)), 0.0), 30.0);

	// new color value
	// color = 0.15 * color + 0.85 * calculateShadow(lightPosition);
	float shadow = calculateShadow(lightPosition, normal);

	vec4 finalColor = vec4((color-shadow) * diffuse, 1.0) + vec4(vec3(1.0) * specular, 0.0);

	float FOG_DENSITY = 0.05;
	float fogDistance = gl_FragCoord.z / gl_FragCoord.w;
	float fogAmount = fogFactorExp2(fogDistance, FOG_DENSITY);
	vec4 fogColor = vec4(1.0); // white

	gl_FragColor = mix(finalColor, fogColor, fogAmount);


//	gl_FragColor = vec4((color-shadow) * diffuse, 1.0) + vec4(vec3(1.0) * specular, 0.0);
	//gl_FragColor = vec4(shadow, shadow, shadow, 1.0);

	//gl_FragColor = vec4(color * diffuse, 1.0) + vec4(vec3(1.0) * specular, 0.0);
}
