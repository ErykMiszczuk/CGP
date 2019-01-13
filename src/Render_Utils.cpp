#include "Render_Utils.h"

#include <algorithm>

#include "glew.h"
#include "freeglut.h"

void Core::DrawVertexArray(const float * vertexArray, int numVertices, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Core::DrawVertexArrayIndexed( const float * vertexArray, const int * indexArray, int numIndexes, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, indexArray);
}


void Core::DrawVertexArray( const VertexData & data )
{
	int numAttribs = std::min(VertexData::MAX_ATTRIBS, data.NumActiveAttribs);
	for(int i = 0; i < numAttribs; i++)
	{
		glVertexAttribPointer(i, data.Attribs[i].Size, GL_FLOAT, false, 0, data.Attribs[i].Pointer);
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLES, 0, data.NumVertices);
}

void Core::DrawModel( obj::Model * model )
{
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, &model->vertex[0]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, &model->texCoord[0]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, &model->normal[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	unsigned short * tmp = &model->faces["default"][0];
	glDrawElements(GL_TRIANGLES, model->faces["default"].size(), GL_UNSIGNED_SHORT, tmp);
}

void Core::DrawModelV(obj::Model * model)
{
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, &model->vertex[0]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, &model->texCoord[0]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, &model->normal[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	unsigned short * tmp = &model->faces["default"][0];
	glDrawElements(GL_TRIANGLES, model->faces["default"].size(), GL_UNSIGNED_SHORT, tmp);
}

void Core::DrawModelT(obj::Model * model, float * tang)
{
	//for (int i = 0; i < model->normal.size(); i += 3)printf("tangent %f %f %f \n", tang[i + 0], tang[i + 1], tang[i + 2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, &model->vertex[0]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, &model->texCoord[0]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, &model->normal[0]);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, tang);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);


	//for (int i = 0; i < model->normal.size(); i += 3)printf("tangent %f %f %f \n", tang[i + 0], tang[i + 1], tang[i + 2]);
	unsigned short * tmp = &model->faces["default"][0];
	//printf("%i %i\n", model->normal.size(), tang->size());
	glDrawElements(GL_TRIANGLES, model->faces["default"].size(), GL_UNSIGNED_SHORT, tmp);
}
