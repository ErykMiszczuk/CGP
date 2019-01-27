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

const vec3 DiffuseLight = vec3(0.15, 0.05, 0.0);
const vec3 RimColor  = vec3(0.2, 0.2, 0.2);

float fogFactorVertex=0;
const vec3 fogColor = vec3(0.5,0.5,0.5);

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
	vec3 albedo = texture2D(textureColor, modifiedTexCoord).rgb*1.5;
	
	
	/* Materials */
	
	vec3 lightColors = vec3(300.0);
	ao = 11.0;

	metallic = texture2D(textureMetallic, interpTexCoord).r;
	roughness = texture2D(textureRoughness, interpTexCoord).g;

    vec3 N = normalize(interpNormal);
    vec3 V = normalize(cameraPos - interpPos);

    vec3 F0 = vec3(0.045);
	
	// calculate F0 value taking material properties into account
    F0 = mix(F0, albedo, metallic);

    // calculate radiance
    vec3 L = normalize(lightPosition.xyz - interpPos);
    vec3 H = normalize(V + L);
    float distance    = length(lightPosition.xyz - interpPos);
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


	/* Fog */
	//diffuse lighting
	vec3 vecdiffuse = DiffuseLight * max(0, dot(L, interpPosWorld));
	
	//rim lighting
	
	float rim = 1 - max(dot(V, interpPosWorld), 0.0);
	rim = smoothstep(0.6, 1.0, rim);
	vec3 finalRim = RimColor * vec3(rim, rim, rim);


	//get all lights and texture
	vec3 finalColor = finalRim + vecdiffuse + albedo;

	vec3 c = vec3(0,0,0);
	float dist = 0;
	float fogFactor = 0;
   
	/*if(depthFog == 1) {
     	if(depthFogChanges == 0){
			dist = abs(interpPos.z);
		} else {
			dist = (gl_FragCoord.z / gl_FragCoord.w);
		}
	} else {
		dist = length(interpPos);
	}*/
	
	dist = (gl_FragCoord.z / gl_FragCoord.w);

	//float be = (10.0 - interpPos.y)*0.004;
	//float bi = (10.0 - interpPos.y)*0.001;

	float be = 0.025 * smoothstep(0.0, 6.0, 10.0 - interpPos.y);
	float bi = 0.035 * smoothstep(0.0, 80, 10.0 - interpPos.y);
	float ext =  exp(-dist * be);
	float insc = exp(-dist * bi);
		
	c = finalColor * ext + fogColor * (1 - insc);
	
	//fogFactor = 1 - fogFactor;
	//out_color = vec4( fogFactor, fogFactor, fogFactor, 1.0);



	/* Normal map */
	
	vec3 normal = texture2D(textureNormal, modifiedTexCoord).rgb;
	normal = (normalize(normal) - 0.5f) * 2.0f;

	float diffuse = max(dot(normal, -lightWorld), 0.0);
	
	vec3 toEye = normalize(cameraPos - interpPos);
	float specular = pow(max(dot(toEye, reflect(lightDir, normal)), 0.0), 30.0);
	
	
	/* Shadow */
	
	float shadow = calculateShadow(lightPosition, normal);
	
	//c = mix(fogColor, finalColor, fogFactorVF);
	vec4 colorWithShadow = vec4((color - (1.0 - shadow)) * diffuse, 1.0) + vec4(vec3(1.0) * specular, 1.0);
	gl_FragColor = mix(colorWithShadow, c)
}

//
//
//
//
////particle structure
//struct particle {
//	glm::vec3 location;
//	glm::vec3 speed;
//	glm::mat4 rotation;
//	glm::vec3 rotation_axis;
//};
//std::vector<particle> fishes;
//
//
//// computation based on time step
//float old_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
//void countFishes()
//{
//	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f ;
//	if ((time - old_time) > 1 || (time - old_time) < 0) {
//		old_time = time - 0.1;
//	}
//
//	
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(0.50f, 0.50f, 0.50f, 1.0f);
//
//	glm::mat4 FishModelMatrix;
//	glm::vec3 attractor(60 * sin(time / 2),-120, 60 * cos(time / 2));
//	//glm::vec3 attractor(0,0, 10);
//	float cohesion_distance = 50;
//	float aligment_distance = 40;
//	float separation_distance = 30;
//	float dt = 0.05;
//	float view_angle = 0.2f;
//	std::vector<particle> newParticles;
//	for (auto &adam : fishes) {
//		particle eva;
//		eva.speed = adam.speed;
//		glm::vec3 cohesion(0.0, 0.0, 0.0);
//		glm::vec3 aligment(0.0, 0.0, 0.0);
//		float cohesion_size = 0;
//		float aligment_size = 0;
//		for (auto &lilit : fishes) {
//			float distance = glm::length(adam.location - lilit.location);
//			if (distance < cohesion_distance && distance > 0.01f && glm::dot(glm::normalize(adam.speed), glm::normalize(lilit.location - adam.location)) > view_angle) {
//				if (distance < aligment_distance) {
//					aligment += (lilit.speed - adam.speed);
//					aligment_size += 1;
//				}
//				cohesion_size += 1;
//				cohesion += glm::normalize(lilit.location - adam.location);
//			}
//		}
//		cohesion_size = (cohesion_size > 1) ? cohesion_size : 1;
//		aligment_size = (aligment_size > 1) ? aligment_size : 1;
//		glm::vec3 separation(0.0, 0.0, 0.0);
//		float separation_size = 0;
//		for (auto &lilit : fishes) {
//			float distance = glm::length(adam.location - lilit.location);
//			if (glm::length(adam.location - lilit.location) < separation_distance && distance > 0.001f) {
//				separation_size += 1;
//				separation -= (lilit.location - adam.location) / (distance*distance);
//			}
//		}
//		separation_size = (separation_size > 1) ? separation_size : 1;
//		glm::vec3 xxx(0);
//		if (glm::length(attractor - adam.location) > 0) {
//			xxx = (attractor - adam.location)*glm::inversesqrt(glm::length(attractor - adam.location));
//		}
//		// modifier when fish is above water
//		if (eva.location.y > 0) {
//			eva.speed -= glm::vec3(0, 100 * (time - old_time), 0);
//		}
//		else {
//			eva.speed = eva.speed + (40.0f * separation + 0.20f * cohesion / cohesion_size + 0.300f * aligment / aligment_size + 10 * xxx)*(time - old_time);
//		}
//		eva.location = adam.location + eva.speed*(time - old_time);
//		
//		/*
//		XXXX   XXX  XXXXX  XXX  XXXXX  XXX   XXX  X   X
//		X   X X	  X   X   X   X   X     X   X   X XX  X
//		XXXX  X   X   X   XXXXX   X     X   X   X X X X
//		X	X X   X   X   X   X   X     X   X   X X  XX
//		X	X  XXX    X   X   X   X    XXX   XXX  X   X
//		*/
//		//importart part for adding rotation
//		glm::vec3 rotation_axis = glm::normalize(glm::cross(glm::vec3(0.0, 0.0, 1.0), glm::normalize(eva.speed)));
//		float angle = glm::acos(glm::dot(rotation_axis, glm::normalize(eva.speed)));
//		//myfile << angle << '\n';
//		//myfile << rotation_axis.x << ' ' << rotation_axis.y << ' ' << rotation_axis.z << ' ' << '\n';
//
//		if (glm::length(glm::cross(glm::vec3(0.0, 0.0, 1.0), glm::normalize(eva.speed))) < 0.00001) {
//			rotation_axis = glm::vec3(0.0, 0.0, 1.0);
//			angle = 0.0;
//			eva.rotation = adam.rotation;
//		}
//		else {
//			glm::mat4 rotation = glm::rotate(angle, rotation_axis);
//			eva.rotation = glm::toMat4(glm::normalize(glm::quat(adam.rotation) + glm::quat(rotation)*0.5f));
//		}
//		newParticles.push_back(eva);
//	}
//	fishes = newParticles;
//	old_time = time;
//}
////function that returns fish model matrix 
//glm::mat4 fish_model_matrix(particle fish) {
//	//be aware that in this code location and fish model are scaled
//	return glm::translate(fish.location / 10.0)*fish.rotation*glm::scale(glm::vec3(0.25))*glm::translate(glm::vec3(2.5, 0, -1));
//}
//// fish initialisation
//void init_fish() {
//
//
//	//create fish particles
//	//...
//	int i = 100;
//	while (i--) {
//		particle p;
//		p.location = glm::vec3(glm::linearRand(-10, 10), glm::linearRand(-220, -185), glm::linearRand(-10, 10));
//		p.speed = glm::vec3(0.0, 0.0, 30.0);
//		p.rotation_axis = glm::vec3(0.0, 0.0, 1.0);
//		p.rotation = glm::mat4(1);
//		fishes.push_back(p);
//	}
//
//}