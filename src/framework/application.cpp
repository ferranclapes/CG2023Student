#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include "entity.h"
#include "texture.h"

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
	this->zBuffer.Resize(w, h);
}

Application::~Application()
{
	SDL_DestroyWindow(window);
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	//bool toolbar_l = toolbar.LoadPNG("../res/images/toolbar.png", false);	//tires enrere i entres al res
	//bool foto_l = fruits.LoadPNG("../res/images/fruits.png", true);
	mesh_anna = new Mesh();
	mesh_lee = new Mesh();
	mesh_cleo = new Mesh();
	texture_anna = new Image();
	texture_lee = new Image();
	texture_cleo = new Image();
	bool mesh_l = mesh_lee->LoadOBJ("meshes/lee.obj");
	bool mesh_a = mesh_anna->LoadOBJ("meshes/anna.obj");
	bool mesh_c = mesh_cleo->LoadOBJ("meshes/cleo.obj");
	bool tex_a = texture_anna->LoadTGA("textures/anna_color_specular.tga",true);
	bool tex_l = texture_lee->LoadTGA("textures/lee_color_specular.tga", true);
	bool tex_c = texture_cleo->LoadTGA("textures/cleo_color_specular.tga", true);
	if (/*toolbar_l == false || foto_l == false || */mesh_l == false || mesh_a == false|| mesh_c == false|| tex_a == false || tex_l == false || tex_c == false)
	{
		exit(0);
	}
	anna = Entity(mesh_anna, Vector3(10, -9, -70), 0*DEG2RAD, Vector3(0, 1, 0), Vector3(30, 30, 30), texture_anna);
	lee = Entity(mesh_lee, Vector3(0, -5, 0), 0 * DEG2RAD, Vector3(0, 1, 0), Vector3(15, 15, 15), texture_lee);
	cleo = Entity(mesh_cleo, Vector3(-10, -9, 0), 0 * DEG2RAD, Vector3(0, 1, 0), Vector3(30, 30, 30), texture_cleo);
	camera = Camera();
	camera.LookAt(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0,1,0));
	camera.SetOrthographic(20, -20, 10, -5, 1, -100);

	color = Color::WHITE;
	desp = 0.1;
	scal = 0.005;
	is_t = true;
	fov = 45;

	menu = 1;
	menu_c = 1;
}

// Render one frame
void Application::Render(void)
{
	framebuffer.Fill(Color::BLACK);
	zBuffer.Fill(100.0);
	switch (menu)
	{
	case 1:
		anna.Render_3(&framebuffer, &camera, Color::RED, &zBuffer, texture_anna, menu);
		lee.Render_3(&framebuffer, &camera, Color::BLUE, &zBuffer, texture_lee, menu);
		cleo.Render_3(&framebuffer, &camera, Color::GREEN, &zBuffer, texture_cleo, menu);
		break;

	case 2:
		anna.Render_3(&framebuffer, &camera, Color::RED, &zBuffer, texture_anna, menu);
		lee.Render_3(&framebuffer, &camera, Color::BLUE, &zBuffer, texture_lee, menu);
		cleo.Render_3(&framebuffer, &camera, Color::GREEN, &zBuffer, texture_cleo, menu);
		break;

	case 3:
		anna.Render_3(&framebuffer, &camera, Color::RED, &zBuffer, texture_anna, menu);
		lee.Render_3(&framebuffer, &camera, Color::BLUE, &zBuffer, texture_lee, menu);
		cleo.Render_3(&framebuffer, &camera, Color::GREEN, &zBuffer, texture_cleo, menu);
		break;

	case 4:
		anna.Render_3(&framebuffer, &camera, Color::RED, &zBuffer, texture_anna, menu);
		lee.Render_3(&framebuffer, &camera, Color::BLUE, &zBuffer, texture_lee, menu);
		cleo.Render_3(&framebuffer, &camera, Color::GREEN, &zBuffer, texture_cleo, menu);
		break;
	}
	framebuffer.Render();
}

// Called after render
void Application::Update(float seconds_elapsed)
{


	//Lab 2
	/*int t = floor(time);
	float s = 1 + scal;
	switch (menu)
	{
	case 1:
		if (t % 4 == 0 && is_t == true)
		{
			desp *= -1;
			scal *= -1;
			is_t = false;

		}
		else if (t % 4 != 0)
		{
			is_t = true;
		}
		anna.Update(Vector3(0, 0, 0), 30 * DEG2RAD, Vector3(0, 1, 0), Vector3(1, 1, 1), seconds_elapsed);
		lee.Update(Vector3(desp, 0, 0), 0, Vector3(0, 0, 0), Vector3(1, 1, 1), seconds_elapsed);
		cleo.Update(Vector3(0, 0, 0), 0, Vector3(0, 0, 0), Vector3(s, s, s), seconds_elapsed);
		break;

	case 2:
		break;

	}*/

}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event) {
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode

	//Lab 2
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: exit(0); break;
	case SDLK_p:
		camera.SetPerspective(fov * DEG2RAD, framebuffer.width / framebuffer.height, 1, 50);
		break;

	case SDLK_o:
		camera.SetOrthographic(20, -20, 10, -5, 1, -500);
		break;

	case SDLK_1:
		menu = 1;
		break;

	case SDLK_2:
		menu = 2;
		break;

	case SDLK_3:
		menu = 3;
		break;

	case SDLK_4:
		menu = 4;
		break;

	case SDLK_n:
		is_near = true;
		break;

	case SDLK_f:
		is_near = false;

	case SDLK_PLUS:
		if (is_near)
		{
			camera.near_plane++;
		}
		else
		{
			camera.far_plane++;
		}
		camera.UpdateProjectionMatrix();
		break;

	case SDLK_MINUS:
		if (is_near)
		{
			camera.near_plane--;
		}
		else
		{
			camera.far_plane--;
		}
		camera.UpdateProjectionMatrix();
		break;

	case SDLK_b:
		if (camera.type == Camera::PERSPECTIVE)
		{

			if (fov >= 170)
			{
				break;
			}
			fov += 5;
			camera.fov = fov * DEG2RAD;
			camera.UpdateProjectionMatrix();
		}
		break;

	case SDLK_s:
		if (camera.type == Camera::PERSPECTIVE)
		{
			if (fov <= 25)
			{
				break;
			}
			fov -= 5;
			camera.fov = fov * DEG2RAD;
			camera.UpdateProjectionMatrix();

		}
		break;

	case SDLK_m:
		menu_c = 1;
		break;

	case SDLK_r:
		menu_c = 2;
		break;

	case SDLK_c:
		menu_c = 3;
		break;
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
  	if (event.button == SDL_BUTTON_LEFT) {
		int m;
	}
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	if (event.type == SDL_MOUSEWHEEL)
	{
		camera.Zoom(Vector3(0, 0, event.y));
	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT)
	{
		if (menu_c == 1)
		{
			camera.Move(Vector3(-0.1 * mouse_delta.x, 0.1 * mouse_delta.y, 0));
		}
		else if (menu_c == 2)
		{

			camera.Rotate(0.1*mouse_delta.x * DEG2RAD, Vector3(0, 1, 0));
			camera.Rotate(0.1 * mouse_delta.y * DEG2RAD, Vector3(1, 0, 0));
		}
		else if (menu_c == 3)
		{
			camera.Orbit(0.1*mouse_delta.x * DEG2RAD, Vector3(0, 1, 0));
		}
	}
	
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}