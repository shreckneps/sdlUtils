#ifndef SDLUTILS_H
#define SDLUTILS_H


#include <math.h>
#include "SDL.h"
//#include "SDL2/SDL.h"

//computes the square of the euclidean distance between two input points
float SDLUtils_DistanceSquared(SDL_FPoint p1, SDL_FPoint p2);

//renders the input vertices as a triangle fan, with the first vertex being the center
int SDLUtils_RenderTriFan(SDL_Renderer *renderer,
                          SDL_Texture *texture,
                          const SDL_Vertex *vertices, int num_vertices,
                          int closed);

//renders the input vertices as a triangle strip
int SDLUtils_RenderTriStrip(SDL_Renderer *renderer,
                            SDL_Texture *texture,
                            const SDL_Vertex *vertices, int num_vertices);


//draws a circle with the given center and radius, with perimeter lines of no longer than maxSideLength
int SDLUtils_RenderDrawCircleF(SDL_Renderer *renderer,
                               SDL_FPoint center, float radius,
                               float maxSideLength);

//fills a circle with the given center and radius, with perimeter lines of no longer than maxSideLength
int SDLUtils_RenderFillCircleF(SDL_Renderer *renderer,
                               SDL_FPoint center, float radius,
                               float maxSideLength);


//internal function to guarantee an array has sufficient size
void *sdu_ensureLength(void *p, size_t each, int *actualLength, int desiredLength);

//internal function to calculate the size of step angles for circle drawing
float sdu_calculateStepAngle(float radius, float maxSideLength, int *pointCount);

//internal function to find points along a circle's perimeter
void sdu_findCirclePoints(SDL_FPoint center, float radius, 
                          float stepAngle, 
                          SDL_FPoint *points, int pointCount);

#endif
