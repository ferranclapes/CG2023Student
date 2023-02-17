#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

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
	bool mesh_l = mesh_lee->LoadOBJ("meshes/lee.obj");
	bool mesh_a = mesh_anna->LoadOBJ("meshes/anna.obj");
	bool mesh_c = mesh_cleo->LoadOBJ("meshes/cleo.obj");
	if (/*toolbar_l == false || foto_l == false || */mesh_l == false || mesh_a == false)
	{
		exit(0);
	}
	anna = Entity(mesh_anna, Vector3(10, -9, 70), 0, Vector3(1, 1, 1), Vector3(30, 30, 30));
	lee = Entity(mesh_lee, Vector3(0, -5, 0), 90 * DEG2RAD, Vector3(0, 1, 0), Vector3(15, 15, 15));
	cleo = Entity(mesh_cleo, Vector3(-10, -9, 0), 180 * DEG2RAD, Vector3(0, 1, 0), Vector3(30, 30, 30));
	camera = Camera();
	camera.LookAt(Vector3(0, 0, -30), Vector3(0, 0, 0), Vector3(0,1,0));
	camera.SetOrthographic(-20, 20, 10, -5, 1, 100);

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
	//Tot lo del lab 1. Després cal descomentar-ho
	/*framebuffer.DrawImagePixels(toolbar, 0, 0, true);


	switch (menu) {
	case 0:		//Borrar tot
		framebuffer.Fill(Color::BLACK);
		menu = -1;
		break;

	case 1:		//Linia DDA
		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawLineDDA(pos0->x, pos0->y, pos->x, pos->y, Color::WHITE);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 2:		//Linia Bresenham
		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawLineBresenham(pos0->x, pos0->y, pos->x, pos->y, Color::WHITE);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 3:		//Cercle buit
		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawCircle(pos0->x, pos0->y, pos->Distance(*pos0), Color::WHITE, false);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 4:		//Cercle ple
		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawLineDDA(pos0->x, pos0->y, pos->x, pos->y, color);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 2:

		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawLineBresenham(pos0->x, pos0->y, pos->x, pos->y, color);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 3:

		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawCircle(pos0->x, pos0->y, pos->Distance(*pos0), color, false);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 4:

		if (pos->y == -1)
		{
			break;
		}
		framebuffer.DrawCircle(pos0->x, pos0->y, pos->Distance(*pos0), color, true);
		pos0->set(-1, -1);
		pos->set(-1, -1);
		menu = -1;
		break;

	case 5:
		break;

	case 6:
		framebuffer.DrawImagePixels(fruits, 0, 0, false);
		framebuffer.DrawImagePixels(toolbar, 0, 0, true);
		break;

	case 7:
		framebuffer.Fill(Color::BLACK);
		for (int i = 0; i < num_particle; i++)
		{
			if (abs(particles[i].pos.Distance(particles[i].origin))> 75)
			{
				framebuffer.DrawLineBresenham(particles[i].pos.x, particles[i].pos.y, particles[i].pos.x - particles[i].size.x, particles[i].pos.y - particles[i].size.y, particles[i].color);

			}
		}
		break;
	}
	*/
	framebuffer.Fill(Color::BLACK);
	anna.Render(&framebuffer, &camera, Color::RED);
	lee.Render(&framebuffer, &camera, Color::BLUE);
	cleo.Render(&framebuffer, &camera, Color::GREEN);
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

	//Tot lo del lab 1
	/*if (menu == 7)
	{
		for (int i = 0; i < num_particle; i++)
		{
			if (time > 40)
			{
				time = 40;
			}
			particles[i].pos += particles[i].vel * time * seconds_elapsed;
			if (particles[i].pos.Distance(particles[i].origin) >= particles[i].dest.Distance(particles[i].origin))
			{
				particles[i].pos = particles[i].origin;
			}
		}
	}*/
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: exit(0); break;
	case SDLK_0:
		//Lab2
		/*
		anna = Entity(mesh_anna, Vector3(10, -9, 50), 0, Vector3(1, 1, 1), Vector3(30, 30, 30));
		lee = Entity(mesh_lee, Vector3(0, -5, 0), 90 * DEG2RAD, Vector3(0, 1, 0), Vector3(15, 15, 15));
		cleo = Entity(mesh_cleo, Vector3(-10, -9, 0), 180 * DEG2RAD, Vector3(0, 1, 0), Vector3(30, 30, 30));
		camera = Camera();
		camera.LookAt(Vector3(0, 0, -25), Vector3(0, 0, 0), Vector3(0, 1, 0));
		camera.SetOrthographic(-20, 20, 10, -5, 1, 500);

		color = Color::WHITE;
		desp = 0.1;
		scal = 0.005;
		is_t = true;
		fov = 45;

		menu = 1;
		menu_c = 1;
		break;
		*/
	case SDLK_p:
		camera.SetPerspective(fov * DEG2RAD, framebuffer.width / framebuffer.height, 1, 50);
		break;

	case SDLK_o:
		camera.SetOrthographic(-20, 20, 10, -5, 1, 500);
		break;

	case SDLK_1:
		menu = 1;
		break;

	case SDLK_2:
		menu = 2;
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

	//Lab 1
	/*switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		//A PARTIR D'AQUI COMENÇA EL CODI DELS ALUMNES

	case SDLK_0:
		menu = 0;
		break;

	case SDLK_1:		//Premer tecla 1 per linia dda
		menu = 1;
		break;

	case SDLK_2:		//Premer tecla 2 per linia bresenham
		menu = 2;
		break;

	case SDLK_3:		//Premer tecla 3 per cercle buit
		menu = 3;
		break;

	case SDLK_4:		//Premer tecla 4 per cercle ple
		menu = 4;
		break;

	case SDLK_5:		//Premer tecla 5 per dibuix lliure
		menu = 5;
		brush_size = 0;
		break;

	case SDLK_6:
		menu = 6;
		break;

	case SDLK_7:
		menu = 7;
		for (int i = 0; i < num_particle; i++)
		{
			Particle particle;

			particle.origin = Vector2(framebuffer.width / 2, framebuffer.height / 2);

			particle.quartil = 1 + (rand() % 4);
			int desti_rand = rand() % 2;
			if (particle.quartil == 4)
			{
				if (desti_rand == 0)
				{
					particle.dest = Vector2(0, (framebuffer.height / 2) + (rand() % (framebuffer.height / 2)));
				}
				else
				{
					particle.dest = Vector2((rand() % (framebuffer.width / 2)), framebuffer.height);
				}

			}
			else if (particle.quartil == 1)
			{
				if (desti_rand == 0)
				{
					particle.dest = Vector2(framebuffer.width, (framebuffer.height / 2) + (rand() % (framebuffer.height / 2)));
				}
				else
				{
					particle.dest = Vector2((framebuffer.width / 2) + (rand() % (framebuffer.width / 2)), framebuffer.height);
				}
			}
			else if (particle.quartil == 2)
			{
				if (desti_rand == 0)
				{
					particle.dest = Vector2(framebuffer.width, (rand() % (framebuffer.height / 2)));
				}
				else
				{
					particle.dest = Vector2((framebuffer.width / 2) + (rand() % (framebuffer.width / 2)), 0);
				}
			}
			else if (particle.quartil == 3)
			{
				if (desti_rand == 0)
				{
					particle.dest = Vector2(0, (rand() % (framebuffer.height / 2)));
				}
				else
				{
					particle.dest = Vector2((rand() % (framebuffer.width / 2)), 0);
				}
			}

			particle.pos = particle.origin;

			int size = 1 + (rand() % 30);
			particle.size = Vector2(size * ((particle.dest.x - particle.origin.x) / particle.dest.Distance(particle.origin)),
				size * ((particle.dest.y - particle.origin.y) / particle.dest.Distance(particle.origin)));
			if (size > 25)
			{
				particle.color = Color::WHITE;

				int aux_v = 4 + (rand() % 3);
				particle.vel = Vector2((particle.size.x * aux_v) / aux_v, (particle.size.y * aux_v) / aux_v);
			}
			else if (size > 17)
			{
				particle.color = Color(20, 75, 120);

				int aux_v = 4 + (rand() % 3);
				particle.vel = Vector2((particle.size.x * aux_v) / aux_v, (particle.size.y * aux_v) / aux_v);
			}
			else if (size > 10)
			{
				particle.color = Color(120, 70, 20);

				int aux_v = 1 + (rand() % 3);
				particle.vel = Vector2((particle.size.x * aux_v) / aux_v, (particle.size.y * aux_v) / aux_v);
			}
			else
			{
				particle.color = Color::GRAY;

				int aux_v = 1 + (rand() % 2);
				particle.vel = Vector2((particle.size.x * aux_v) / aux_v, (particle.size.y * aux_v) / aux_v);
			}
			particles[i] = particle;
		}
		time = 5;
		break;

	case SDLK_LSHIFT:
		color = framebuffer.GetPixel(mouse_position.x, mouse_position.y);
		break;

	case  SDLK_PLUS:
		brush_size += 2;
		break;

	case SDLK_MINUS:
		if (brush_size == 0)
		{
			break;
		}
		brush_size -= 2;
		break;
	}*/

}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	//Lab 1
	/*if (event.button == SDL_BUTTON_LEFT) {
		if (pos0->x == -1 || this->es_primer_clic == true)
		{
			pos0->set(mouse_position.x, mouse_position.y);
			es_primer_clic = false;
		}
		else
		{
			pos->set(mouse_position.x, mouse_position.y);
			es_primer_clic = true;
		}
	}*/
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
	
	//Lab 1
	/*if (event.button == SDL_BUTTON_LEFT && menu == 5)
	{
		int x = mouse_position.x - brush_size / 2;
		int y = mouse_position.y + brush_size / 2;
		while (y >= mouse_position.y - brush_size / 2)
		{
			while (x <= mouse_position.x + brush_size / 2)
			{
				framebuffer.SetPixelSafe(x, y, color);
				x++;
			}
			y--;
			x = mouse_position.x - brush_size / 2;
		}
	}*/
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}