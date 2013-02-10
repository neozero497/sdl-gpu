#include "SDL.h"
#include "SDL_gpu.h"
#include <math.h>

#ifdef SDL_GPU_USE_SDL2
#define SDL_GetKeyState SDL_GetKeyboardState
#endif

#define PI 3.14159265359

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


void getScreenToWorld(float screenX, float screenY, float* worldX, float* worldY)
{
	GPU_Camera camera = GPU_GetCamera();
	GPU_Target* screen = GPU_GetDisplayTarget();
	if(screen == NULL)
		return;
	
	if(worldX)
	{
		//if(camera.angle == 0.0f)
			*worldX = (screenX - screen->w/2) / camera.zoom + camera.x + screen->w/2;
		//else
			//*worldX = (screenX - screen->w/2) / camera.zoom * cos(-camera.angle*PI/180) - (screenY - screen->h/2) / camera.zoom * sin(-camera.angle*PI/180) + camera.x + screen->w/2;
	}
	if(worldY)
	{
		//if(camera.angle == 0.0f)
			*worldY = (screenY - screen->h/2) / camera.zoom + camera.y + screen->h/2;
		//else
			//*worldY = (screenX - screen->w/2) / camera.zoom * sin(-camera.angle*PI/180) + (screenY - screen->h/2) / camera.zoom * cos(-camera.angle*PI/180) + camera.y + screen->h/2;
	}
}

void getWorldToScreen(float worldX, float worldY, float* screenX, float* screenY)
{
	GPU_Camera camera = GPU_GetCamera();
	GPU_Target* screen = GPU_GetDisplayTarget();
	if(screen == NULL)
		return;
	
	if(screenX)
	{
		//if(camera.angle == 0.0f)
			*screenX = (worldX - camera.x - screen->w/2)*camera.zoom + screen->w/2;
		//else
			//*screenX = (worldX - camera.x - screen->w/2)*camera.zoom * cos(-camera.angle*PI/180) + screen->w/2;
	}
	if(screenY)
	{
		//if(camera.angle == 0.0f)
			*screenY = (worldY - camera.y - screen->h/2)*camera.zoom + screen->h/2;
		//else
			//*screenY = (worldY - camera.y - screen->h/2)*camera.zoom * sin(-camera.angle*PI/180) + screen->h/2;
	}
}

void printScreenToWorld(float screenX, float screenY)
{
	float worldX, worldY;
	getScreenToWorld(screenX, screenY, &worldX, &worldY);
	
	printf("ScreenToWorld: (%.1f, %.1f) -> (%.1f, %.1f)\n", screenX, screenY, worldX, worldY);
}

void printWorldToScreen(float worldX, float worldY)
{
	float screenX, screenY;
	getWorldToScreen(worldX, worldY, &screenX, &screenY);
	
	printf("WorldToScreen: (%.1f, %.1f) -> (%.1f, %.1f)\n", worldX, worldY, screenX, screenY);
}


int main(int argc, char* argv[])
{
	printRenderers();
	
	GPU_Target* screen = GPU_Init(NULL, 800, 600, 0);
	if(screen == NULL)
		return -1;
	
	printf("Using renderer: %s\n", GPU_GetCurrentRendererID());
	
	
	Uint32 startTime = SDL_GetTicks();
	long frameCount = 0;
	
	
	SDL_Color black = {0, 0, 0, 255};
	
	GPU_Image* img = GPU_LoadImage("data/test3.png");
	
	Uint8* keystates = SDL_GetKeyState(NULL);
	
	GPU_Camera camera = GPU_GetDefaultCamera();
	
	float dt = 0.010f;
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
				else if(event.key.keysym.sym == SDLK_r)
				{
					camera.x = 0.0f;
					camera.y = 0.0f;
					camera.z = -10.0f;
					camera.zoom = 1.0f;
					camera.angle = 0.0f;
				}
				else if(event.key.keysym.sym == SDLK_SPACE)
				{
					int mx, my;
					SDL_GetMouseState(&mx, &my);
					float x, y;
					GPU_GetVirtualCoords(&x, &y, mx, my);
					
					printf("Angle: %.1f\n", camera.angle);
					printScreenToWorld(x, y);
					printWorldToScreen(50, 50);
				}
				else if(event.key.keysym.sym == SDLK_w)
				{
					camera.y -= 100;
				}
				else if(event.key.keysym.sym == SDLK_s)
				{
					camera.y += 100;
				}
				else if(event.key.keysym.sym == SDLK_a)
				{
					camera.x -= 100;
				}
				else if(event.key.keysym.sym == SDLK_d)
				{
					camera.x += 100;
				}
			}
		}
		
		if(keystates[SDLK_UP])
		{
			camera.y -= 200*dt;
		}
		else if(keystates[SDLK_DOWN])
		{
			camera.y += 200*dt;
		}
		if(keystates[SDLK_LEFT])
		{
			camera.x -= 200*dt;
		}
		else if(keystates[SDLK_RIGHT])
		{
			camera.x += 200*dt;
		}
		if(keystates[SDLK_z])
		{
			camera.z -= 200*dt;
		}
		else if(keystates[SDLK_x])
		{
			camera.z += 200*dt;
		}
		if(keystates[SDLK_MINUS])
		{
			camera.zoom -= 1.0f*dt;
		}
		else if(keystates[SDLK_EQUALS])
		{
			camera.zoom += 1.0f*dt;
		}
		if(keystates[SDLK_COMMA])
		{
			camera.angle -= 100*dt;
		}
		else if(keystates[SDLK_PERIOD])
		{
			camera.angle += 100*dt;
		}
		
		GPU_ClearRGBA(screen, 255, 255, 255, 255);
		
		GPU_SetCamera(screen, &camera);
		
		GPU_Rect(screen, 0, 0, 800, 600, black);
		GPU_Blit(img, NULL, screen, 50, 50);
		GPU_Blit(img, NULL, screen, 320, 50);
		GPU_Blit(img, NULL, screen, 50, 500);
		
		
		GPU_Flip();
		
		frameCount++;
		if(frameCount%500 == 0)
			printf("Average FPS: %.2f\n", 1000.0f*frameCount/(SDL_GetTicks() - startTime));
	}
	
	printf("Average FPS: %.2f\n", 1000.0f*frameCount/(SDL_GetTicks() - startTime));
	
	GPU_FreeImage(img);
	
	GPU_Quit();
	
	return 0;
}

