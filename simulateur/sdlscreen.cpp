#ifdef WITHSDL

#include "sdlscreen.h"


SDLScreen::SDLScreen():
    _refresh(false),
_force_quit(false)
{
    //createsdl();
    _thread = std::thread(&SDLScreen::action, this);
}

SDLScreen::~SDLScreen() {
    this->_force_quit = true;
    _thread.join();
    //closesdl();
}

void SDLScreen::updateContent(uword *memory) {
    _memory = memory;
    _refresh = true;
    while (_refresh) {};
}

void SDLScreen::createsdl() {
	SDL_Init(SDL_INIT_VIDEO);
	_window = SDL_CreateWindow("Asm", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH*2, HEIGHT*2, 0);
	_renderer = SDL_CreateRenderer(_window, -1, 0);
	_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
}

void SDLScreen::closesdl() {
    if (_texture)
	SDL_DestroyTexture(_texture);
    _texture = 0;
    if (_renderer)
	SDL_DestroyRenderer(_renderer);
    _renderer = 0;
    if (_window)
	SDL_DestroyWindow(_window);
    _window = 0;
	SDL_Quit();
}
    
void SDLScreen::action() {
    this->createsdl();
	SDL_Event e;
	Uint32 last_time = SDL_GetTicks();
	SDL_RenderSetScale(_renderer, 2, 2);
	bool escape = false;
	uint32_t tempscreen[0xffff - MEM_SCREEN_BEGIN + 1];

	while (!escape) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				escape = true;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					escape = true;
			}
		}
		if (_force_quit)
			escape = _force_quit;
		if (_refresh) {
			for (unsigned int i = MEM_SCREEN_BEGIN; i <= 0xFFFF; ++i) {
				word pixel = (_memory) ? _memory[i] : 0;
				uint32_t blue = pixel & ((1<<5)-1); 
				uint32_t green = (pixel>>5) & ((1<<5)-1); 
				uint32_t red = (pixel>>10) ;
				tempscreen[i - MEM_SCREEN_BEGIN] = (red << (2+16)) + (green << (3+8)) + (blue << 3);
			}
			SDL_UpdateTexture(_texture, NULL, tempscreen, WIDTH * sizeof(uint32_t));
			SDL_RenderClear(_renderer);
			SDL_RenderCopy(_renderer, _texture, NULL, NULL);
			SDL_RenderPresent(_renderer);
			Uint32 current = SDL_GetTicks();
			if (current - last_time < (1000.f * 1.f/60.f)) {
				SDL_Delay((1000.f * 1.f / 60.f) - last_time + current);
			}
			_refresh = false;
			last_time = current;
		}
	}
	_refresh = false;
    this->closesdl();
}

#endif
