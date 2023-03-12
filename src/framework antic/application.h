/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "entity.h"

class Application
{
public:
	//Lab3:
	FloatImage zBuffer;
	Image texture_lee;
	Image texture_anna;
	Image texture_cleo;


	//Lab2:
	Mesh* mesh_anna;
	Mesh* mesh_lee;
	Mesh* mesh_cleo;
	Entity anna;
	Entity lee;
	Entity cleo;
	Camera camera;

	bool is_t;

	float desp;
	float scal;

	float fov;

	int menu_c;
	int is_near;

	//Lab 1 i defoult:
	int menu;
	int brush_size;
	Vector2* pos;
	Vector2* pos0;
	Image toolbar;
	Image fruits;
	Color color;
	static const int num_particle = 700;
	bool es_primer_clic;

	struct Particle {
		int quartil;
		Vector2 pos;
		Vector2 origin;
		Vector2 dest;
		Vector2 vel;
		Vector2 size;
		Color color;
	};
	Particle particles[num_particle] = {};
	// Window

	SDL_Window* window;
	int window_width;
	int window_height;

	float time;

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		//this->framebuffer.width = width-1;
		//this->framebuffer.height = height-1;
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}
};
