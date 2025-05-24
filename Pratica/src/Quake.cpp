//
// Quake map viewer
//
// Author: Johan Gardhage <johan.gardhage@gmail.com>
//
#include <SDL2/SDL.h>
#include <iomanip>
#include "imgui.h"
#include <cstdint>
#include <getopt.h> // getopt_long
#include "Map.h"
#include "World.h"
#include "Camera.h"
#include <sstream>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <libgen.h>
#include<stdio.h>


bool useFISR = false;
bool fKeyPressed = false;
bool useTraditional = false;
bool tKeyPressed = false;


enum Parameters
{
	WINDOW = 1,
	FULLWINDOW = 2,
	FULLSCREEN = 4,
	SHOWCURSOR = 8
};

/* void RenderBenchmarks(float fisr_time, float slow_time) {
    // Debug: Print what we're trying to render
    static int debug_counter = 0;
    if (debug_counter++ % 60 == 0) { // Print every 60 frames (about 1 second at 60fps)
        printf("[DEBUG] RenderBenchmarks called with fisr_time=%.2f, slow_time=%.2f\n", fisr_time, slow_time);
    }

    // Always show some text, even if timing is 0
    std::ostringstream oss;
    if (fisr_time == 0.0f && slow_time == 0.0f) {
        oss << "DEBUG: Benchmarks not running (F to toggle FISR: " << (useFISR ? "ON" : "OFF") << ")";
    } else {
        oss << std::fixed << std::setprecision(2)
            << "FISR: " << fisr_time << "us  Slow: " << slow_time << "us";
    }

} */

int ParseArguments(int argc, char *argv[], int param)
{
	int parameters = param;
	static struct option long_options[] = {
		{"help",       no_argument, 0, 'h'},
		{"window",     no_argument, 0, 'w'},
		{"fullwindow", no_argument, 0, 0},
		{"fullscreen", no_argument, 0, 'f'},
		{"showcursor", no_argument, 0, 'c'},
		{"nocursor",   no_argument, 0, 0},
		{0,            0,           0, 0}
	};
	bool usage = false;
	int c;
	int option_index = 0;
	while ((c = getopt_long(argc, argv, ":hwfc", long_options, &option_index)) != -1) {
		switch (c) {
		case 0:
			if (strcmp("fullwindow", long_options[option_index].name) == 0) {
				parameters |= FULLWINDOW;
				parameters &= ~(WINDOW | FULLSCREEN);
			} else if (strcmp("nocursor", long_options[option_index].name) == 0) {
				parameters &= ~SHOWCURSOR;
			}
			break;
		case 'h':
			usage = true;
			break;
		case 'w':
			parameters |= WINDOW;
			parameters &= ~(FULLWINDOW | FULLSCREEN);
			break;
		case 'f':
			parameters |= FULLSCREEN;
			parameters &= ~(WINDOW | FULLWINDOW);
			break;
		case 'c':
			parameters |= SHOWCURSOR;
			break;
		case '?':
			usage = true;
			printf("unrecognized option '%s'\n", argv[optind - 1]);
			break;
		default:
			usage = true;
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}
	if (optind < argc) {
		usage = true;
		printf ("non-option ARGV-elements: ");
		while (optind < argc) {
			printf("%s ", argv[optind++]);
		}
		printf("\n");
	}
	if (usage) {
		printf("Usage: %s [OPTION]...\n\n", basename(argv[0]));
		printf("Options:\n");
		printf(" -h, --help         Display this text and exit\n");
		printf(" -w, --window       Render in a window\n");
		printf("     --fullwindow   Render in a fullscreen window\n");
		printf(" -f, --fullscreen   Render in fullscreen\n");
		printf(" -c, --showcursor   Show mouse cursor\n");
		printf("     --nocursor     Hide mouse cursor\n");
		printf("Press F to turn on the FISR algorithm\n");
		printf("Press T to turn on the traditional algorithm\n");
		exit(1);
	}
	return parameters;
}

int main(int argc, char *argv[])
{
	Map map;
	World world;
	Camera camera;

	// Parse arguments
	int parameters = ParseArguments(argc, argv, FULLSCREEN);

	// Load map
	if (!map.Initialize("../assets/start.bsp", "../assets/palette.lmp")) {
		printf("[ERROR] Quake::main() Unable to initialize map\n");
		return -1;
	}

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("[ERROR] Quake::main() Unable to initialize SDL\n");
		return -1;
	}

	// Get current display mode
	SDL_DisplayMode dm;
	if (SDL_GetCurrentDisplayMode(0, &dm) != 0) {
		printf("[ERROR] Quake::main() Unable to get current display mode\n");
		return -1;
	}
	if (parameters & WINDOW) {
		dm.w = 800;
		dm.h = 600;
	}

	// Cursor and mouse mode
	if (parameters & SHOWCURSOR) {
		SDL_ShowCursor(1);
	} else {
		SDL_ShowCursor(0);

		// Set relative mouse mode
		if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0) {
			printf("[ERROR] Quake::main() Unable to set mouse mode\n");
			return -1;
		}
	}

	// Create window
	Uint32 flags = SDL_WINDOW_OPENGL;
	if (parameters & FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	SDL_Window *window = SDL_CreateWindow("Quake!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dm.w, dm.h, flags);
	if (!window) {
		printf("[ERROR] Quake::main() Unable to create window\n");
		return -1;
	}

	// Create our opengl context and attach it to our window
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context) {
		printf("[ERROR] Quake::main() Unable to create context\n");
		return -1;
	}

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 150");



	// Initialize world
	if (!world.Initialize(&map, dm.w, dm.h)) {
		printf("[ERROR] Quake::main() Unable to initialize world\n");
		return -1;
	}

	bool loop = true;
	while (loop) {
		SDL_PumpEvents();

		const Uint8 * keys = SDL_GetKeyboardState(NULL);

		// Handle keyboard events
		if (SDL_QuitRequested()) {
			loop = false;
			break;
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_ESCAPE)]) {
			loop = false;
			break;
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_q)]) {
			loop = false;
			break;
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_w)]) {
			camera.MoveForward();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_UP)]) {
			camera.MoveForward();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_s)]) {
			camera.MoveBackward();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_DOWN)]) {
			camera.MoveBackward();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_RIGHT)]) {
			camera.TurnRight();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_LEFT)]) {
			camera.TurnLeft();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_d)]) {
			camera.StrafeRight();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_f)]){
			if (!fKeyPressed){
				useFISR = !useFISR;
				fKeyPressed = true;
				printf("[DEBUG] FISR: %s\n", useFISR ? "ON" : "OFF");
			}

		}

		else {
			fKeyPressed = false;
		}

		if (keys[SDL_GetScancodeFromKey(SDLK_t)]) {
    		if (!tKeyPressed) {
        		useTraditional = !useTraditional;
        		tKeyPressed    = true;
        		printf("[DEBUG] Traditional 1/sqrt: %s\n", useTraditional ? "ON" : "OFF");}
		} 

		else {
			tKeyPressed = false;
		}

		if (keys[SDL_GetScancodeFromKey(SDLK_a)]) {
			camera.StrafeLeft();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_PAGEUP)]) {
			camera.PitchUp();
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_PAGEDOWN)]) {
			camera.PitchDown();
		}

		// Handle mouse events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);  
			if (event.type == SDL_MOUSEMOTION) {
				camera.Yaw(event.motion.xrel);
				camera.Pitch(event.motion.yrel);
			}
		}

		// Update camera position
		camera.UpdatePosition();

		//printf("Drawing scene...\n");
		
		// Draw the scene
		world.DrawScene(&camera);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();


		{
		ImGui::Begin("Benchmark Stats");
		ImGui::Text("FISR:  %.2f µs", ::fisr_time);

		if (useTraditional)
			ImGui::Text("Trad:  %.2f µs", ::slow_time);

		else
			ImGui::Text("Slow:  %.2f µs", ::slow_time);

		ImGui::Separator();
		ImGui::Text("Press F to toggle FISR: %s", useFISR ? "ON" : "OFF");
		ImGui::Text("Press T to toggle Traditional: %s", useTraditional ? "ON" : "OFF");

		ImGui::End();
		}


		//printf("Scene drawn.\n");
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);
	}

	// Delete our OpengL context
	SDL_GL_DeleteContext(context);

	// Destroy our window
	SDL_DestroyWindow(window);


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();


	// Shutdown SDL
	SDL_Quit();

	return 0;
}
