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
float metallic;
float roughness;
float ao;

const float PI = 3.14159265359;

/* Material functions */

float Distribution(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickS1(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickS1(NdotV, roughness);
    float ggx1  = GeometrySchlickS1(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

/* Shadow function */

float calculateShadow(vec4 lightPosition, vec3 normal)
{
	vec3 divided = lightPosition.xyz / lightPosition.w;
	divided = divided * 0.5 + 0.5;
	
	float closestDepth = texture2D(depthMap, divided.xy).r;
	float currentDepth = divided.z;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(depthMap, divided.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 13.0;
	
	if(currentDepth > 1.0)
        shadow = 0.0;

	return shadow;
}

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y);
	vec3 albedo = texture2D(textureColor, modifiedTexCoord).rgb;
	
	
	/* Materials */
	
	lightColors = vec3(300.0);
	lightPos = vec3(10.0);
	ao = 11.0;

	metallic = texture2D(textureMetallic, interpTexCoord).r;
	roughness = texture2D(textureRoughness, interpTexCoord).g;

    vec3 N = normalize(interpNormal);
    vec3 V = normalize(cameraPos - interpPos);

    vec3 F0 = vec3(0.045);
	
	// calculate F0 value taking material properties into account
    F0 = mix(F0, albedo, metallic);

    // calculate radiance
    vec3 L = normalize(lightPos - interpPos);
    vec3 H = normalize(V + L);
    float distance    = length(lightPos - interpPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = lightColors * attenuation; 
	
	//here starts BSDF magick
	vec3 F = fresnelSchlick(dot(L,H), F0);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
        
	float G = GeometrySchlickS1(dot(N,H), 0.5);
	float D = Distribution(N,H, 0.5);
	vec3 materialsSpecular = (F * G * D) / (4* dot(N,L) * dot(N,V));
            
    // calculate outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    vec3 Lo =  4.0*(kD * albedo / PI + materialsSpecular) * radiance * NdotL; 
  
    vec3 ambient = vec3(0.01) * albedo * ao;
    vec3 color = ambient + Lo;
	
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.0));  
   


	/* Normal map */
	
	vec3 normal = texture2D(textureNormal, modifiedTexCoord).rgb;
	normal = (normalize(normal) - 0.5f) * 2.0f;

	float diffuse = max(dot(normal, -lightWorld), 0.0);
	
	vec3 toEye = normalize(cameraPos - interpPos);
	float specular = pow(max(dot(toEye, reflect(lightDir, normal)), 0.0), 30.0);
	
	
	/* Shadow */
	
	float shadow = calculateShadow(lightPosition, normal);
	
	gl_FragColor = vec4((color - (1.0 - shadow)) * diffuse, 1.0) + vec4(vec3(1.0) * specular, 1.0);
}