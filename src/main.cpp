#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor;
GLuint programDepth;
GLuint programTexture;
GLuint programShadow;
GLuint programNormal;
GLuint programNormalShadow;
GLuint programSkybox;

GLuint depthTexture;
GLuint FramebufferObject;


Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
obj::Model renderModel;
obj::Model dalekModel;
obj::Model fishModel;
obj::Model bottomPlaneModel;

glm::vec3 cameraPos = glm::vec3(0, 0, 14);
glm::vec3 cameraDir;
glm::vec3 cameraSide;
float cameraAngle = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::vec3 rotationChangeXYZ = glm::vec3(0, 0, 0);

GLuint textureAsteroid;
GLuint textureEarth, textureEarthNormal;
GLuint textureDalek;
GLuint skybox;
GLuint textureFish;
GLuint textureBottomPlane;
GLuint textureSink;
GLuint textureShip, textureShipNormal, textureShipMetallic, textureShipSmoothness;

GLuint cubemapTexture;

std::vector<glm::vec4> planets;
std::vector<float> tangent(1203);

glm::vec3 fishVectors[10];

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
int NUM_FISH = 10;

const float cubeVertices[] = {
	30.5f, 30.5f, 30.5f, 1.0f,
	30.5f, -30.5f, 30.5f, 1.0f,
	-30.5f, 30.5f, 30.5f, 1.0f,

	30.5f, -30.5f, 30.5f, 1.0f,
	-30.5f, -30.5f, 30.5f, 1.0f,
	-30.5f, 30.5f, 30.5f, 1.0f,

	30.5f, 30.5f, -30.5f, 1.0f,
	-30.5f, 30.5f, -30.5f, 1.0f,
	30.5f, -30.5f, -30.5f, 1.0f,

	30.5f, -30.5f, -30.5f, 1.0f,
	-30.5f, 30.5f, -30.5f, 1.0f,
	-30.5f, -30.5f, -30.5f, 1.0f,

	-30.5f, 30.5f, 30.5f, 1.0f,
	-30.5f, -30.5f, 30.5f, 1.0f,
	-30.5f, -30.5f, -30.5f, 1.0f,

	-30.5f, 30.5f, 30.5f, 1.0f,
	-30.5f, -30.5f, -30.5f, 1.0f,
	-30.5f, 30.5f, -30.5f, 1.0f,

	30.5f, 30.5f, 30.5f, 1.0f,
	30.5f, -30.5f, -30.5f, 1.0f,
	30.5f, -30.5f, 30.5f, 1.0f,

	30.5f, 30.5f, 30.5f, 1.0f,
	30.5f, 30.5f, -30.5f, 1.0f,
	30.5f, -30.5f, -30.5f, 1.0f,

	30.5f, 30.5f, -30.5f, 1.0f,
	30.5f, 30.5f, 30.5f, 1.0f,
	-30.5f, 30.5f, 30.5f, 1.0f,

	30.5f, 30.5f, -30.5f, 1.0f,
	-30.5f, 30.5f, 30.5f, 1.0f,
	-30.5f, 30.5f, -30.5f, 1.0f,

	30.5f, -30.5f, -30.5f, 1.0f,
	-30.5f, -30.5f, 30.5f, 1.0f,
	30.5f, -30.5f, 30.5f, 1.0f,

	30.5f, -30.5f, -30.5f, 1.0f,
	-30.5f, -30.5f, -30.5f, 1.0f,
	-30.5f, -30.5f, 30.5f, 1.0f,
};

float quad[] = {
	0.25,0.25, 0.0, 1.0,
	0.25, -0.25, 0.0, 1.0,
	-0.25, -0.25, 0.0, 1.0,

	0.25, 0.25, 0.0, 1.0,
	-0.25, 0.25, 0.0, 1.0,
	-0.25, -0.25, 0.0, 1.0
};

float poprzedniaPozycjaMyszki[2] = { 0,0 };
float roznicaMyszki[2] = { 0,0 };

void keyboard(unsigned char key, int x, int y)
{
	float nowaPozycja[2] = { x, y };

	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
	float nowaPozycja[2] = { x, y };
	roznicaMyszki[0] = poprzedniaPozycjaMyszki[0] - nowaPozycja[0];
	roznicaMyszki[1] = poprzedniaPozycjaMyszki[1] - nowaPozycja[1];

	if (glm::length(glm::vec2(roznicaMyszki[x], roznicaMyszki[y])) > 2.f) {
		poprzedniaPozycjaMyszki[0] = nowaPozycja[0];
		poprzedniaPozycjaMyszki[1] = nowaPozycja[1];
		return;
	}
	poprzedniaPozycjaMyszki[0] = nowaPozycja[0];
	poprzedniaPozycjaMyszki[1] = nowaPozycja[1];
}

glm::mat4 createCameraMatrix()
{
	glm::quat obrotY = glm::angleAxis(-roznicaMyszki[0] * 0.005f, glm::vec3(0, 1, 0));
	glm::quat obrotX = glm::angleAxis(-roznicaMyszki[1] * 0.005f, glm::vec3(1, 0, 0));

	glm::quat rotationChange = obrotX * obrotY;

	rotation = rotationChange * rotation;
	rotation = glm::normalize(rotation);

	roznicaMyszki[0] = 0;
	roznicaMyszki[1] = 0;

	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos, rotation);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);


	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	
	Core::DrawModel(model);


	glUseProgram(0);
}

void drawSkybox(glm::mat4 modelMatrix, GLuint skyboxId)
{
	//todo
	GLuint program = programSkybox;

	glUseProgram(program);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::VertexData vertexData;
	vertexData.NumActiveAttribs = 1;
	vertexData.Attribs[0].Pointer = cubeVertices;
	vertexData.Attribs[0].Size = 4;
	vertexData.NumVertices = 36;
	//not sure about these values...

	//binding skybox texture
	glUniform1i(glGetUniformLocation(program, "cubeSampler"), skyboxId);
	glActiveTexture(GL_TEXTURE0 + skyboxId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxId);

	Core::DrawVertexArray(vertexData);
	glUseProgram(0);
}

void createDepthMap(obj::Model * model, glm::mat4 modelMatrix)
{
	GLuint program = programDepth;

	glUseProgram(program);


	glm::mat4 projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
	glm::mat4 inverseLigthDirection = glm::lookAt(-lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 lightMVP = projectionMatrix * inverseLigthDirection * modelMatrix;


	glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&lightMVP);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTextureShadows(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programShadow;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(depthTexture, "depthMap", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;

	glm::mat4 projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
	glm::mat4 inverseLigthDirection = glm::lookAt(-lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 lightMVP = projectionMatrix * inverseLigthDirection * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&lightMVP);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTextureNormal(obj::Model * model, glm::mat4 modelMatrix, GLuint texture, GLuint normalMap)
{
	GLuint program = programNormal;

	glUseProgram(program);

	Core::SetActiveTexture(texture, "textureColor", program, 0);
	Core::SetActiveTexture(normalMap, "textureNormal", program, 1);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glm::mat4 ModelViewMatrix = cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&ModelViewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModelT(model, &tangent[0]);

	glUseProgram(0);
}

void drawObjectTextureNormalShadow(obj::Model * model, glm::mat4 modelMatrix, GLuint texture, GLuint normalMap)
{
	GLuint program = programNormalShadow;

	glUseProgram(program);

	Core::SetActiveTexture(texture, "textureColor", program, 0);
	Core::SetActiveTexture(normalMap, "textureNormal", program, 1);
	// new
	Core::SetActiveTexture(depthTexture, "depthMap", program, 2);
	//

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;

	glm::mat4 projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
	glm::mat4 inverseLigthDirection = glm::lookAt(-lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 lightMVP = projectionMatrix * inverseLigthDirection * modelMatrix;
	// new
	glm::mat4 ModelViewMatrix = cameraMatrix * modelMatrix;
	//
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&lightMVP);
	// new
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&ModelViewMatrix);
	//

	//GLuint program = programNormal;

	//glUseProgram(program);

	/*Core::SetActiveTexture(texture, "textureColor", program, 0);
	Core::SetActiveTexture(normalMap, "textureNormal", program, 1);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);*/

	//glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	//glm::mat4 ModelViewMatrix = cameraMatrix * modelMatrix;
	//glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	//glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&ModelViewMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModelT(model, &tangent[0]);
	//Core::DrawModel(model);
/*
*	To podejœcie niestety nie dzia³a, bd musia³ zmiksowaæ dwa shadery jednak. No ale có¿, próbowa³em...
*	zostanie jeszcze do poprawki sam cieñ, bo jest pikselowy. No i ta trawa... chwilê to jeszcze zajmie
*/

	glUseProgram(0);
	//GLuint program = programNormalShadow;

	//glUseProgram(program);

	//Core::SetActiveTexture(texture, "textureColor", program, 0);
	//Core::SetActiveTexture(normalMap, "textureNormal", program, 1);
	////Core::SetActiveTexture(texture, "textureSampler", program, 2);
	//Core::SetActiveTexture(depthTexture, "depthMap", program, 2);

	//glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	//glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	//glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	//glm::mat4 ModelViewMatrix = cameraMatrix * modelMatrix;

	//glm::mat4 projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
	//glm::mat4 inverseLigthDirection = glm::lookAt(-lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//glm::mat4 lightMVP = projectionMatrix * inverseLigthDirection * modelMatrix;

	//glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	//glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&ModelViewMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&lightMVP);

	//Core::DrawModelT(model, &tangent[0]);

	//glUseProgram(0);
}

void drawObjectUberShader(obj::Model * model, glm::mat4 modelMatrix, GLuint texture, GLuint normalMap, GLuint metallic, GLuint smoothness)
{
	GLuint program = programNormalShadow;

	glUseProgram(program);

	Core::SetActiveTexture(texture, "textureColor", program, 0);
	Core::SetActiveTexture(normalMap, "textureNormal", program, 1);
	Core::SetActiveTexture(depthTexture, "depthMap", program, 2);
	Core::SetActiveTexture(metallic, "textureMetallic", program, 3);
	Core::SetActiveTexture(smoothness, "textureRoughness", program, 4);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;

	glm::mat4 projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
	glm::mat4 inverseLigthDirection = glm::lookAt(-lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 lightMVP = projectionMatrix * inverseLigthDirection * modelMatrix;
	glm::mat4 ModelViewMatrix = cameraMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMVP"), 1, GL_FALSE, (float*)&lightMVP);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&ModelViewMatrix);


	Core::DrawModelT(model, &tangent[0]);

	glUseProgram(0);
}

void renderScene()
{
	
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferObject);
	  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	  
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glm::mat4 projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
	glm::mat4 inverseLigthDirection = glm::lookAt(-lightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 planetModelMatrix = glm::translate(glm::vec3(sin(-time), 2.0, cos(-time))) * glm::scale(glm::vec3(3.0));
	glm::mat4 dalekModelMatrix = glm::translate(glm::vec3(-2.5, 4.0, 0.0)) * glm::scale(glm::vec3(0.025));
	glm::mat4 renderTarget = glm::translate(glm::vec3(0, -5, 0)) * glm::rotate(-1.56f, glm::vec3(1, 0, 0)) * glm::rotate(1.56f, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(7, 14, 14));
	glm::mat4 lightMVP = projectionMatrix * inverseLigthDirection;

	/////////////
	glm::mat4 shipRotation = glm::mat4_cast(glm::inverse(rotation));

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * shipRotation * shipInitialTransformation;

	glm::mat4 sink = glm::translate(glm::vec3(0, -2.5f, 0)) * glm::rotate(glm::radians(100.0f), glm::vec3(1, 1, 0)) * glm::scale(glm::vec3(2.0f));
	/////////////

	glCullFace(GL_FRONT);
	int iterm = -108;
	int loopi = 0;
	for (loopi = 0; loopi < 3; loopi++) {
		createDepthMap(&bottomPlaneModel, glm::translate(glm::vec3(-108, -5, iterm)));
		createDepthMap(&bottomPlaneModel, glm::translate(glm::vec3(0, -5, iterm)));
		createDepthMap(&bottomPlaneModel, glm::translate(glm::vec3(108, -5, iterm)));
		iterm += 108;
	}
	//createDepthMap(&renderModel, renderTarget);
	//createDepthMap(&sphereModel, planetModelMatrix);
	createDepthMap(&dalekModel, dalekModelMatrix);
	createDepthMap(&shipModel, shipModelMatrix);
	createDepthMap(&shipModel, sink);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//skybox
	drawSkybox(glm::translate(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z)), skybox);
	//
	//drawObjectTextureShadows(&renderModel, renderTarget, textureAsteroid);
	drawObjectTextureShadows(&dalekModel, dalekModelMatrix, textureDalek);
	//drawObjectTextureShadows(&sphereModel, planetModelMatrix, textureEarth);
	//drawObjectTextureNormal(&sphereModel, planetModelMatrix, textureEarth, textureEarthNormal);
	//drawObjectTextureNormalShadow(&sphereModel, planetModelMatrix, textureEarth, textureEarthNormal);
	iterm = -108;
	loopi = 0;
	for (loopi = 0; loopi < 3; loopi++) {
		drawObjectTextureShadows(&bottomPlaneModel, glm::translate(glm::vec3(-108, -5, iterm)), textureBottomPlane);
		drawObjectTextureShadows(&bottomPlaneModel, glm::translate(glm::vec3(0, -5, iterm)), textureBottomPlane);
		drawObjectTextureShadows(&bottomPlaneModel, glm::translate(glm::vec3(108, -5, iterm)), textureBottomPlane);
		iterm += 108;
	}
	drawObjectUberShader(&shipModel, shipModelMatrix, textureShip, textureShipNormal, textureShipMetallic, textureShipSmoothness);
	drawObjectUberShader(&shipModel, sink, textureSink, textureShipNormal, textureShipMetallic, textureShipSmoothness);
	//

	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_depth.vert", "shaders/shader_depth.frag");
	programShadow = shaderLoader.CreateProgram("shaders/shader_shadow.vert", "shaders/shader_shadow.frag");
	programNormal = shaderLoader.CreateProgram("shaders/shader_normal.vert", "shaders/shader_normal.frag");
	programNormalShadow = shaderLoader.CreateProgram("shaders/shader_normalshadow.vert", "shaders/shader_normalshadow.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	renderModel = obj::loadModelFromFile("models/render.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	dalekModel = obj::loadModelFromFile("models/dalek1.obj");

	textureEarth = Core::LoadTexture("textures/earth.png");
	textureAsteroid = Core::LoadTexture("textures/ypos.png");
	textureDalek = Core::LoadTexture("textures/dalek/cdb88b60.png");
	textureEarthNormal = Core::LoadTexture("textures/earth_normalmap.png");

	fishModel = obj::loadModelFromFile("models/ryba.obj");
	shipModel = obj::loadModelFromFile("models/OrcaSub1.obj");
	textureShip = Core::LoadTexture("textures/orca_sub_red.png");
	textureSink = Core::LoadTexture("textures/orca_sub_sink.png");
	textureFish = Core::LoadTexture("textures/rybauvmap.png");
	bottomPlaneModel = obj::loadModelFromFile("models/bottom1.obj");
	textureBottomPlane = Core::GenerateTexture(100, 100, 30, 1);
	textureShipMetallic = Core::LoadTexture("textures/orca_sub_met.png");
	textureShipSmoothness = Core::LoadTexture("textures/orca_sub_smo.png");
	textureShipNormal = Core::LoadTexture("textures/orca_sub_nor.png");

	skybox = Core::setupCubeMap("textures/xpos.png", "textures/xneg.png", "textures/ypos.png", "textures/yneg.png", "textures/zpos.png", "textures/zneg.png");


	std::vector<float> unitY = { 0.0, 1.0, 0.0 };
	std::vector<float> unitX = { 1.0, 0.0, 0.0 };

	glm::vec3 uY = { 0.0, -1.0, 0.0 };
	glm::vec3 uX = { -1.0, 0.0, 0.0 };

	glm::vec3 tang[1203];

	for (int i = 0; i < sphereModel.normal.size(); i += 3)
	{

		glm::vec3 normal = { sphereModel.normal[i + 0], sphereModel.normal[i + 1], sphereModel.normal[i + 2] };

		if (sphereModel.normal[i + 1] < 0.99 && sphereModel.normal[i + 1] > -0.99) tang[i] = glm::normalize(glm::cross(normal, uY));
		else tang[i] = glm::normalize(glm::cross(normal, uX));


		tangent[i + 0] = tang[i].x;
		tangent[i + 1] = tang[i].y;
		tangent[i + 2] = tang[i].z;
		
	}


	//Generate Framebuffer
	FramebufferObject = 0;
	glGenFramebuffers(1, &FramebufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferObject); 

	//Generate depth texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//In case of things outside thie view frustum
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Attach depth texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("CGP");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
