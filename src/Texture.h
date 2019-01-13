#pragma once

#include "glew.h"
#include "freeglut.h"

namespace Core
{
	GLuint LoadTexture(const char * filepath);
	GLuint LoadTextureCM(const char * filepath);
	GLuint setupCubeMap(const char *xpos, const char *xneg, const char *ypos, const char *yneg, const char *zpos, const char *zneg);

	// textureID - identyfikator tekstury otrzymany z funkcji LoadTexture
	// shaderVariableName - nazwa zmiennej typu 'sampler2D' w shaderze, z ktora ma zostac powiazana tekstura
	// programID - identyfikator aktualnego programu karty graficznej
	// textureUnit - indeks jednostki teksturujacej - liczba od 0 do 7. Jezeli uzywa sie wielu tekstur w jednym shaderze, to kazda z nich nalezy powiazac z inna jednostka.
	void SetActiveTexture(GLuint textureID, const char * shaderVariableName, GLuint programID, int textureUnit);

	GLuint GenerateTexture(int var_r, int var_g, int var_b, int var_a);
	float perlin(float x, float y);
	float interpolatedNoise(float x, float y);
	float SmoothedNoise1(float x, float y);
	float interpol(float a, float b, float x);
	float noise1(int x, int y);

}