#include <stdlib.h>
#include "sdlUtils.h"

//internal function to guarantee an array has sufficient size
void *sdu_ensureLength(void *p, size_t each, int *actualLength, int desiredLength) {
    //SDL_Log("ensure length called, actual %d, desired %d\n", *actualLength, desiredLength);
    if(desiredLength > *actualLength) {
        //if(p) { free(p); }
        p = realloc(p, (each * desiredLength));
        if(p) { 
            //SDL_Log("alloced %d\n", desiredLength);
        } else {
            SDL_Log("Failure! Could not realloc.\n");
        }
        *actualLength = desiredLength;
    }
    return p;
}

//internal function to calculate the size of step angles for circle drawing
float sdu_calculateStepAngle(float radius, float maxSideLength, int *pointCount) {
    SDL_FPoint p1, p2;
    float retAngle;
    int distinctCount;

    p1.x = radius;
    p1.y = 0;
    
    //calculating square of distance easier than calculating distance
    //so use the square of provided MSL rather than the MSL itself
    maxSideLength *= maxSideLength;
    
    retAngle = 3.1415926f;
    distinctCount = 2;
    
    do {
        retAngle /= 2;
        distinctCount *= 2; 

        p2.x = radius * cosf(retAngle);
        p2.y = radius * sinf(retAngle);
    } while(SDLUtils_DistanceSquared(p1, p2) > maxSideLength);
    
    //add one to support including the start-point twice
    *pointCount = distinctCount + 1;
    return retAngle;
}

//internal function to find points along a circle's perimeter
void sdu_findCirclePoints(SDL_FPoint center, float radius, 
                          float stepAngle, 
                          SDL_FPoint *points, int pointCount) {
    int i;
    float theta;
    SDL_FPoint p;

    p.x = center.x + radius;
    p.y = center.y;
    
    //SDL_Log("point address: %x\n", points);
    //SDL_Log("point count: %d\n", pointCount);
    points[0] = p;
    points[pointCount - 1] = p;

    for(i = 1; i < pointCount - 1; i++) {
        //SDL_Log("i: %d\n", i);
        theta = i * stepAngle;
        p.x = center.x + radius * cosf(theta);
        p.y = center.y + radius * sinf(theta);
        points[i] = p;
    }
}


//computes the square of the euclidean distance between two input points
float SDLUtils_DistanceSquared(SDL_FPoint p1, SDL_FPoint p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return (dx * dx) + (dy * dy);
}

//renders the input vertices as a triangle fan, with the first vertex being the center
int SDLUtils_RenderTriFan(SDL_Renderer *renderer,
                          SDL_Texture *texture,
                          const SDL_Vertex *vertices, int num_vertices,
                          int closed) {
    //using static array variables reduces the amount of reallocation of memory needed
    //does prevent any sort of thread safety, but sdl render already thread unsafe
    static int *indexArray = NULL;
    static int indexArraySize = 0;
    int num_indices;
    int i;
    
    //triangle fan with 3 vertices has 1 triangle
    //every vertex after that adds 1 more triangle
    //plus 1 more triangle if closed
    num_indices = num_vertices - 2;
    if(closed) { num_indices++; }
    num_indices *= 3;
    indexArray = (int *)sdu_ensureLength(indexArray, sizeof(int), &indexArraySize, num_indices);
    
    //i is the i-th triangle in the fan
    for(i = 0; i < num_vertices - 2; i++) {
        //every triangle has the center vertex, and two rolling vertices
        indexArray[3 * i + 0] = 0;
        indexArray[3 * i + 1] = i + 1;
        indexArray[3 * i + 2] = i + 2;
    }
    if(closed) {
        //if closed, extra triangle has center, first, and last vertices
        indexArray[num_indices - 3] = 0;
        indexArray[num_indices - 2] = 1;
        indexArray[num_indices - 1] = num_vertices - 1;
    }


    return SDL_RenderGeometry(renderer, texture, vertices, num_vertices, indexArray, num_indices);
}

//renders the input vertices as a triangle strip
int SDLUtils_RenderTriStrip(SDL_Renderer *renderer,
                            SDL_Texture *texture,
                            const SDL_Vertex *vertices, int num_vertices) {
    static int *indexArray = NULL;
    static int indexArraySize = 0;
    int num_indices;
    int i;
    
    num_indices = num_vertices - 2;
    num_indices *= 3;
    indexArray = (int *)sdu_ensureLength(indexArray, sizeof(int), &indexArraySize, num_indices);
    
    //i is the i-th triangle in the strip
    for(i = 0; i < num_vertices - 2; i++) {
        //every triangle has three rolling vertices
        //this doesn't have the same flip-ordering as an OpenGL triangle strip would
        //but doesn't need to, since no normal vectors need be preserved
        indexArray[3 * i + 0] = i + 0;
        indexArray[3 * i + 1] = i + 1;
        indexArray[3 * i + 2] = i + 2;
    }

    return SDL_RenderGeometry(renderer, texture, vertices, num_vertices, indexArray, num_indices);
}


//draws a circle with the given center and radius, with perimeter lines of no longer than maxSideLength
int SDLUtils_RenderDrawCircleF(SDL_Renderer *renderer,
                               SDL_FPoint center, float radius,
                               float maxSideLength) {
    static SDL_FPoint *pointArray = NULL;
    static int pointArraySize = 0;
    
    int pointCount;
    float stepAngle = sdu_calculateStepAngle(radius, maxSideLength, &pointCount);
    
    pointArray = (SDL_FPoint *) sdu_ensureLength(pointArray, sizeof(SDL_FPoint), 
                                                 &pointArraySize, pointCount);
    sdu_findCirclePoints(center, radius, stepAngle, pointArray, pointCount);
    return SDL_RenderDrawLinesF(renderer, pointArray, pointCount);
}

//fills a circle with the given center and radius, with perimeter lines of no longer than maxSideLength
int SDLUtils_RenderFillCircleF(SDL_Renderer *renderer,
                               SDL_FPoint center, float radius,
                               float maxSideLength) {
    static SDL_FPoint *pointArray = NULL;
    static int pointArraySize = 0;
    static SDL_Vertex *vertArray = NULL;
    static int vertArraySize = 0;

    int pointCount, i;
    float stepAngle = sdu_calculateStepAngle(radius, maxSideLength, &pointCount);
    SDL_Color rendColor;
    i = SDL_GetRenderDrawColor(renderer,
                               &rendColor.r,
                               &rendColor.g,
                               &rendColor.b,
                               &rendColor.a);
    if(i) { return i; }
    
    pointArray = (SDL_FPoint *) sdu_ensureLength(pointArray, sizeof(SDL_FPoint), 
                                                 &pointArraySize, pointCount);
    vertArray = (SDL_Vertex *) sdu_ensureLength(vertArray, sizeof(SDL_Vertex), 
                                                &vertArraySize, pointCount);
    sdu_findCirclePoints(center, radius, stepAngle, pointArray, pointCount);
    for(i = 0; i < pointCount; i++) {
        vertArray[i].position = pointArray[i];
        vertArray[i].color = rendColor;
        //TODO -- set vertex texture coordinate?
        //apply any sort of per-renderer color mods which RenderGeometry ignores?
    }
    
    //closed=false, since circle points explicitly includes the origin point twice, auto-closed.
    return SDLUtils_RenderTriFan(renderer, NULL, vertArray, pointCount, 0);
}


