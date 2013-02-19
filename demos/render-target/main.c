#include "SDL.h"
#include "SDL_gpu.h"
#include <math.h>

#ifdef SDL_GPU_USE_SDL2
#define SDL_GetKeyState SDL_GetKeyboardState
#define KEY_UP SDL_SCANCODE_UP
#define KEY_DOWN SDL_SCANCODE_DOWN
#define KEY_LEFT SDL_SCANCODE_LEFT
#define KEY_RIGHT SDL_SCANCODE_RIGHT
#else
#define KEY_UP SDLK_UP
#define KEY_DOWN SDLK_DOWN
#define KEY_LEFT SDLK_LEFT
#define KEY_RIGHT SDLK_RIGHT
#endif

void printRenderers(void)
{
	const char* renderers[GPU_GetNumRegisteredRenderers()];
	GPU_GetRegisteredRendererList(renderers);
	
	printf("Available renderers:\n");
	int i;
	for(i = 0; i < GPU_GetNumRegisteredRenderers(); i++)
	{
		printf("%d) %s\n", i+1, renderers[i]);
	}
}

int main(int argc, char* argv[])
{
	printRenderers();
	
	GPU_Target* screen = GPU_Init(NULL, 800, 600, 0);
	if(screen == NULL)
		return -1;
	
	printf("Using renderer: %s\n", GPU_GetCurrentRendererID());
	
	GPU_Image* image = GPU_LoadImage("data/test.bmp");
	if(image == NULL)
		return -1;
	
	GPU_Image* image2 = GPU_LoadImage("data/test2.bmp");
	if(image2 == NULL)
		return -1;
	
	GPU_Image* image3 = GPU_LoadImage("data/test.bmp");
	if(image3 == NULL)
		return -1;
	
	GPU_Target* target = GPU_LoadTarget(image);
	if(target == NULL)
		return -1;
	
	
	SDL_Color circleColor = {255, 0, 0, 128};
	SDL_Color circleColor2 = {0, 0, 255, 128};
	
	
	Uint32 startTime = SDL_GetTicks();
	long frameCount = 0;
	
	Uint8* keystates = SDL_GetKeyState(NULL);
	if(keystates == NULL)
    {
        printf("Failed to get keyboard state!\n");
        return -2;
    }
    
	int x = 0;
	int y = 0;
	
	Uint8 done = 0;
	SDL_Event event;
	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				done = 1;
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
					done = 1;
			}
		}
		
		if(keystates[KEY_UP])
			y -= 1;
		else if(keystates[KEY_DOWN])
			y += 1;
		if(keystates[KEY_LEFT])
			x -= 1;
		else if(keystates[KEY_RIGHT])
			x += 1;
		
		GPU_Clear(screen);
		
		GPU_Clear(target);
		GPU_Blit(image3, NULL, target, image3->w/2, image3->h/2);
		
		//GPU_BlitScale(image2, NULL, target, x, y, 0.7f, 0.7f);
		GPU_BlitScale(image2, NULL, target, target->w/2, target->h/2, 0.7f, 0.7f);
		//GPU_BlitRotate(image2, NULL, target, x, y, 360*sin(SDL_GetTicks()/1000.0f));
		GPU_BlitTransform(image2, NULL, target, x, y, 360*sin(SDL_GetTicks()/1000.0f), 0.7f*sin(SDL_GetTicks()/2000.0f), 0.7f*sin(SDL_GetTicks()/2000.0f));
		
		GPU_CircleFilled(target, 70, 70, 20, circleColor);
	
		GPU_Blit(image, NULL, screen, image->w/2 + 50, image->h/2 + 50);
		
		GPU_CircleFilled(screen, 50 + 70, 50 + 70, 20, circleColor2);
		
		GPU_Flip();
		
		frameCount++;
		if(frameCount%500 == 0)
		{
			printf("Average FPS: %.2f\n", 1000.0f*frameCount/(SDL_GetTicks() - startTime));
			printf("x,y: (%d, %d)\n", x, y);
		}
	}
	
	printf("Average FPS: %.2f\n", 1000.0f*frameCount/(SDL_GetTicks() - startTime));
	
	GPU_FreeImage(image);
	GPU_Quit();
	
	return 0;
}


