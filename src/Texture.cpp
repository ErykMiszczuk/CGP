#include "Texture.h"

#include <fstream>
#include <iterator>
#include <vector>
#include "picopng.h"

typedef unsigned char byte;

GLuint Core::LoadTexture( const char * filepath )
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::ifstream input( filepath, std::ios::binary );
	std::vector<char> buffer((
		std::istreambuf_iterator<char>(input)), 
		(std::istreambuf_iterator<char>()));

	unsigned long w, h;
	std::vector<unsigned char> decoded;
	decodePNG(decoded, w, h, (unsigned char*)&buffer[0], buffer.size(), true);
	

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}


GLuint Core::setupCubeMap(const char *xpos, const char *xneg, const char *ypos, const char *yneg, const char *zpos, const char *zneg)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);


	


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	std::ifstream input(xpos, std::ios::binary);
	std::vector<char> buffer((std::istreambuf_iterator<char>(input)),(std::istreambuf_iterator<char>()));
	unsigned long w, h;
	std::vector<unsigned char> decoded;
	decodePNG(decoded, w, h, (unsigned char*)&buffer[0], buffer.size(), true);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);
	
	std::ifstream input1(xneg, std::ios::binary);
	std::vector<char> buffer1((std::istreambuf_iterator<char>(input1)),(std::istreambuf_iterator<char>()));
	decodePNG(decoded, w, h, (unsigned char*)&buffer1[0], buffer1.size(), true);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);

	std::ifstream input2(ypos, std::ios::binary);
	std::vector<char> buffer2((std::istreambuf_iterator<char>(input2)), (std::istreambuf_iterator<char>()));
	decodePNG(decoded, w, h, (unsigned char*)&buffer2[0], buffer2.size(), true);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);

	std::ifstream input3(yneg, std::ios::binary);
	std::vector<char> buffer3((std::istreambuf_iterator<char>(input3)), (std::istreambuf_iterator<char>()));
	decodePNG(decoded, w, h, (unsigned char*)&buffer3[0], buffer3.size(), true);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);

	std::ifstream input4(zpos, std::ios::binary);
	std::vector<char> buffer4((std::istreambuf_iterator<char>(input4)), (std::istreambuf_iterator<char>()));
	decodePNG(decoded, w, h, (unsigned char*)&buffer4[0], buffer4.size(), true);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);

	std::ifstream input5(zneg, std::ios::binary);
	std::vector<char> buffer5((std::istreambuf_iterator<char>(input5)), (std::istreambuf_iterator<char>()));
	decodePNG(decoded, w, h, (unsigned char*)&buffer5[0], buffer5.size(), true);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);
	
	return textureID;
}




void Core::SetActiveTexture(GLuint textureID, const char * shaderVariableName, GLuint programID, int textureUnit)
{
	glUniform1i(glGetUniformLocation(programID, shaderVariableName), textureUnit);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}


GLuint Core::GenerateTexture(int var_r, int var_g, int var_b, int var_a)
{

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::vector<unsigned char> decoded;


	int i = 0;
	int j = 0;

	float perl;

	for (i = 0; i < 1024; i++)
	{
		for (j = 0; j < 1024; j++)
		{
			perl = perlin(i, j) * 10000;
			perl = perl / 4;

			decoded.push_back((char)perl / var_r);
			decoded.push_back((char)perl / var_g);
			decoded.push_back((char)perl / var_b);
			decoded.push_back((char)perl / var_a);
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}


float Core::perlin(float x, float y)
{
	float total, p, frequency, amplitude;
	int i, n;
	total = 0.0;
	p = 1.0;
	n = 8;


	for (i = 0; i <= n; i++)
	{
		frequency = powf(2.0, (float)i);
		amplitude = p / (pow(2, i));
		total = total + interpolatedNoise(x*frequency / 20, y*frequency / 20)*amplitude;
	}
	return total;
}

float Core::noise1(int x, int y)
{
	int n;
	n = x + y * 57;
	x = (n << 13) ^ n;
	return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

}

float Core::interpol(float a, float b, float x)
{
	float ft, f;
	ft = x * 3.1415927;
	f = (1 - cos(ft)) * .5;
	return  a * (1 - f) + b * f;
}

float Core::SmoothedNoise1(float x, float y)
{
	float corners, sides, center;
	corners = (noise1(x - 1, y - 1) + noise1(x + 1, y - 1) + noise1(x - 1, y + 1) + noise1(x + 1, y + 1)) / 16;
	sides = (noise1(x - 1, y) + noise1(x + 1, y) + noise1(x, y - 1) + noise1(x, y + 1)) / 8;
	center = noise1(x, y) / 4;
	return corners + sides + center;
}

float Core::interpolatedNoise(float x, float y)
{
	float fractional_X, fractional_Y, v1, v2, v3, v4, i1, i2;
	int integer_X, integer_Y;

	integer_X = (int)x;
	fractional_X = x - integer_X;

	integer_Y = (int)y;
	fractional_Y = y - integer_Y;

	v1 = SmoothedNoise1(integer_X, integer_Y);
	v2 = SmoothedNoise1(integer_X + 1, integer_Y);
	v3 = SmoothedNoise1(integer_X, integer_Y + 1);
	v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1);

	i1 = interpol(v1, v2, fractional_X);
	i2 = interpol(v3, v4, fractional_X);

	return interpol(i1, i2, fractional_Y);

}
