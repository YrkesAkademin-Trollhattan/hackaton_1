#include <stdio.h>
#include <stdint.h>
#include <SDL.h>

void init_window();
void draw_screen();
SDL_Surface *draw_mandelbrot(double x0, double x1, double y0, double y1, int w, int h);

/* State of the program */
struct
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	int w, h;
	double x0, x1, y0, y1;
	double zx0, zy0;
	double clicked_x;
	double clicked_y;
} state;

/*** Main ***/
int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	init_window();

	/* Main loop */
	int running = 1;
	while(running)
	{
		/* First handle incoming events */
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_WINDOWEVENT)
			{
				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						state.w = event.window.data1;
						state.h = event.window.data2;
						break;

					case SDL_WINDOWEVENT_CLOSE:
						running = 0;
						break;
				}
			}

			/* Handle zooming on click and draw with mouse */
			if(event.type == SDL_MOUSEBUTTONDOWN &&
			   event.button.button == SDL_BUTTON_LEFT)
			{
				state.clicked_x = state.x0 +
					event.button.x * (state.x1 - state.x0) / state.w;

				state.clicked_y = state.y0 +
					event.button.y * (state.y1 - state.y0) / state.h;
			}

			if(event.type == SDL_MOUSEBUTTONUP &&
			   event.button.button == SDL_BUTTON_LEFT)
			{
				double x = state.x0 +
					event.button.x * (state.x1 - state.x0) / state.w;

				double y = state.y0 +
					event.button.y * (state.y1 - state.y0) / state.h;

				if(x < state.clicked_x)
				{
					state.x0 = x;
					state.x1 = state.clicked_x;
				}
				else
				{
					state.x1 = x;
					state.x0 = state.clicked_x;
				}

				if(y < state.clicked_y)
				{
					state.y1 = y;
					state.y0 = state.clicked_y;
				}
				else
				{
					state.y0 = y;
					state.y1 = state.clicked_y;
				}
			}

			/* Handle right click with mouse */
			if(event.type == SDL_MOUSEBUTTONUP &&
			   event.button.button == SDL_BUTTON_RIGHT)
			{
				state.zx0 = state.x0 +
					event.button.x * (state.x1 - state.x0) / state.w;

				state.zy0 = state.y0 +
					event.button.y * (state.y1 - state.y0) / state.h;
			}
		}

		draw_screen();

		/*
		 * This extra 0 ms delay yields process and prevents program from
		 * drawing an unreasonable amount of CPU time
		 */
		SDL_Delay(0);
	}

	/* Destroy objects and free memory */
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();

	return 0;
}

/* Initialize window */
void init_window()
{
	state.window = SDL_CreateWindow(
		"My window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_RESIZABLE
	);
	state.w = 640;
	state.h = 480;
	state.x0 = -2;
	state.x1 = 1;
	state.y0 = 1.2;
	state.y1 = -1.2;
	state.zx0 = 0;
	state.zy0 = 0;

	state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED);
}

/* Draw everything to screen based on program state */
void draw_screen()
{
	SDL_Surface *custom_surface;
	SDL_Texture *custom_texture;

	custom_surface = draw_mandelbrot(
		state.x0, state.x1,
		state.y0, state.y1,
		state.w, state.h);

	custom_texture = SDL_CreateTextureFromSurface(state.renderer, custom_surface);
	SDL_RenderCopy(state.renderer, custom_texture, NULL, NULL);
	SDL_DestroyTexture(custom_texture);
	SDL_FreeSurface(custom_surface);

	SDL_RenderPresent(state.renderer);
}

/* Draw mandelbrot fractal */
SDL_Surface *draw_mandelbrot(double x0, double x1, double y0, double y1, int w, int h)
{
	/* (a+bi)*(a+bi) = a² - b² + 2abi */
	/* Create surface w*h with 8 bits red, green, blue and alpha => 32 bits */
	SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

	if(surface == NULL)
		return NULL;

	/* Calculate pixel values */
	uint32_t *pixels = surface->pixels;

	for(int x = 0; x < w; x++)
	{
		for(int y = 0; y < h; y++)
		{
			double fx = x0 + x*(x1-x0)/w;
			double fy = y0 + y*(y1-y0)/h;

			int i;
			float a0 = 0;
			float b0 = 0;

			for(i = 0; i < 255; i++)
			{
				/* z1 = z0² + c */
				float a1 = a0*a0 - b0*b0 + fx;
				float b1 = 2*a0*b0 + fy;
				a0 = a1;
				b0 = b1;

				if(a0*a0 + b0*b0 > 4)
					break;
			}

#if 1
			pixels[w*y + x] = SDL_MapRGBA(surface->format,
			                                - i & 0xf0,
			                                - (i & 0xf) << 4,
			                                ((i & 0xf0) >> 4) * (i & 0xf),
			                                0);
#else
			pixels[w*y + x] = SDL_MapRGBA(surface->format, 255-i, 255-i, 255-i, 0);
#endif
		}
	}

	return surface;
};
