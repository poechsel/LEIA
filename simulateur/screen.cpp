#include "screen.h"


void simulate_screen(const Machine& machine, bool &force_quit, volatile bool &refr, bool activate_refresh_command) {
	/*initialise sdl and create the screen*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Asm", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH*2, HEIGHT*2, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	SDL_Event e;
	Uint32 last_time = SDL_GetTicks();
	SDL_RenderSetScale(renderer, 2, 2);
	bool escape = false;
	/*temp buffer*/
	uint32_t tempscreen[0xffff - MEM_SCREEN_BEGIN + 1];

	while (!escape) {
		/*deal with events*/
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				escape = true;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					escape = true;
			}
		}
		if (force_quit)
			escape = force_quit;
		/* if we need to refresh the screen*/
		if (refr || !activate_refresh_command) {
			/* convert the colors */
			for (unsigned int i = MEM_SCREEN_BEGIN; i <= 0xFFFF; ++i) {
				word pixel = machine.memory[i];
				uint32_t blue = pixel & ((1<<5)-1); 
				uint32_t green = (pixel>>5) & ((1<<5)-1); 
				uint32_t red = (pixel>>10) ;
				tempscreen[i - MEM_SCREEN_BEGIN] = (red << (2+16)) + (green << (3+8)) + (blue << 3);
			}
			/* update the screen */
			SDL_UpdateTexture(texture, NULL, tempscreen, WIDTH * sizeof(uint32_t));
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			/* wait */
			Uint32 current = SDL_GetTicks();
			if (current - last_time < (1000.f * 1.f/60.f)) {
				SDL_Delay((1000.f * 1.f / 60.f) - last_time + current);
			}
			refr = false;
			last_time = current;
		}
	}
	/* close the screen properly */
	refr = false;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


