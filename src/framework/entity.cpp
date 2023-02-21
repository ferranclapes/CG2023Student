#include "entity.h"
#include "image.h"
#include "camera.h"


Entity::Entity()
{
	this->mesh = new Mesh();
	this->model_matrix = Matrix44();
}
Entity::Entity(Mesh* mesh, Vector3 trans, float angle, Vector3 rot, Vector3 sca)
{
	this->mesh = mesh;

	model_matrix.SetIdentity();
	model_matrix.Scale(sca);
	model_matrix.Rotate(angle, rot);
	model_matrix.Translate(trans.x, trans.y, trans.z);
}


void Entity::Update(Vector3 trans, float angle, Vector3 rot, Vector3 sca, float seconds_elapsed)
{
  	trans = trans * seconds_elapsed; angle *= seconds_elapsed;
	model_matrix.TranslateLocal(trans.x, trans.y, trans.z);
	if (angle != 0)
	{
		model_matrix.RotateLocal(angle, rot);
	}
	model_matrix.ScaleLocal(sca);
}

void Entity::Render_3(Image* framebuffer, Camera* camera, const Color& c, FloatImage* zBuffer, Image* texture, int menu)
{
	const std::vector<Vector3>& vertices = mesh->GetVertices();
	const std::vector<Vector2>& uvs = mesh->GetUVs();

	for (int i = 0; i < this->mesh->GetVertices().size(); i += 3)
	{
		if (camera->type == Camera::PERSPECTIVE)
		{
			Vector4 vertex = Vector4(vertices[i].x, vertices[i].y, vertices[i].z, 1);
			Vector4 vertex_world = this->model_matrix * vertex;
			bool negZ;
			Vector3 vertex_clip = camera->ProjectVector(vertex_world.GetVector3(), negZ);
			Vector3 vertex_screen = Vector3(((vertex_clip.x + 1) / 2) * (framebuffer->width - 1), ((vertex_clip.y + 1) / 2) * (framebuffer->height - 1), vertex_clip.z);


			Vector4 vertex_1 = Vector4(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z, 1);
			Vector4 vertex_world_1 = this->model_matrix * vertex_1;
			bool negZ_1;
			Vector3 vertex_clip_1 = camera->ProjectVector(vertex_world_1.GetVector3(), negZ_1);
			Vector3 vertex_screen_1 = Vector3(((vertex_clip_1.x + 1) / 2) * (framebuffer->width - 1), ((vertex_clip_1.y + 1) / 2) * (framebuffer->height - 1), vertex_clip_1.z);


			Vector4 vertex_2 = Vector4(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z, 1);
			Vector4 vertex_world_2 = this->model_matrix * vertex_2;
			bool negZ_2;
			Vector3 vertex_clip_2 = camera->ProjectVector(vertex_world_2.GetVector3(), negZ_2);
			Vector3 vertex_screen_2 = Vector3(((vertex_clip_2.x + 1) / 2) * (framebuffer->width - 1), ((vertex_clip_2.y + 1) / 2) * (framebuffer->height - 1), vertex_clip_2.z);

			if (negZ == false || negZ_1 == false || negZ_2 == false)
			{
				switch (menu)
				{
				case 1:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, c);
					break;

				case 2:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, Color::RED, Color::GREEN, Color::BLUE);
					break;

				case 3:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, Color::RED, Color::BLUE, Color::GREEN, zBuffer, nullptr, Vector2(), Vector2(), Vector2());
					break;

				case 4:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, Color::RED, Color::BLUE, Color::GREEN, zBuffer, texture, uvs[i], uvs[i+1], uvs[i+2]);
					break;
				}
			}
		}
		else
		{

			Vector4 vertex = Vector4(vertices[i].x, vertices[i].y, vertices[i].z, 1);
			Vector4 vertex_world = this->model_matrix * vertex;
			bool out;
			Vector3 vertex_clip = camera->OrthogonalVector(vertex_world.GetVector3(), out);
			Vector3 vertex_screen = Vector3(((vertex_clip.x + 1) / 2) * (framebuffer->width - 1), ((vertex_clip.y + 1) / 2) * (framebuffer->height - 1), vertex_clip.z);

			Vector4 vertex_1 = Vector4(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z, 1);
			Vector4 vertex_world_1 = this->model_matrix * vertex_1;
			bool out_1;
			Vector3 vertex_clip_1 = camera->OrthogonalVector(vertex_world_1.GetVector3(), out_1);
			Vector3 vertex_screen_1 = Vector3(((vertex_clip_1.x + 1) / 2) * (framebuffer->width - 1), ((vertex_clip_1.y + 1) / 2) * (framebuffer->height - 1), vertex_clip_1.z);


			Vector4 vertex_2 = Vector4(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z, 1);
			Vector4 vertex_world_2 = this->model_matrix * vertex_2;
			bool out_2;
			Vector3 vertex_clip_2 = camera->OrthogonalVector(vertex_world_2.GetVector3(), out_2);
			Vector3 vertex_screen_2 = Vector3(((vertex_clip_2.x + 1) / 2) * (framebuffer->width - 1), ((vertex_clip_2.y + 1) / 2) * (framebuffer->height - 1), vertex_clip_2.z);

			if (out == false || out_1 == false || out_2 == false)
			{
				switch (menu)
				{
				case 1:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, c);
					break;

				case 2:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, Color::RED, Color::GREEN, Color::BLUE);
					break;

				case 3:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, Color::RED, Color::BLUE, Color::GREEN, zBuffer, nullptr, Vector2(), Vector2(), Vector2());
					break;

				case 4:
					framebuffer->DrawTriangle(vertex_screen, vertex_screen_1, vertex_screen_2, Color::RED, Color::BLUE, Color::GREEN, zBuffer, texture, uvs[i], uvs[i + 1], uvs[i + 2]);
					break;
				}
			}
		}

	}
}