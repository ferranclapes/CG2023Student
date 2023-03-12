#pragma once

#include "mesh.h"
#include "framework.h"
#include "image.h"

class Entity
{
public:
	Mesh* mesh;
	Matrix44 model_matrix;
	//Constructors
	Entity();
	Entity(Mesh* mesh, Vector3 trans, float angle, Vector3 rot, Vector3 sca);

	void Render_3(Image* framebuffer, Camera* camera, const Color& c, FloatImage* zBuffer, Image* texture, int menu);
	void Update(Vector3 trans, float angle, Vector3 rot, Vector3 sca, float seconds_elapsed);
};
