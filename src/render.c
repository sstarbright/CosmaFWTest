#include "../include/render.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_surface.h>

#define FOG_THICKNESS 4.f
#define FOG_COLOR 5, 20, 5
#define AO_COLOR 0, 0, 0
#define HEIGHT_OFFSET 8
#define AO_SHARPNESS 1.5f
#define AO_BRIGHTNESS .3f

Vector2i* renderMapSize;
Vector2i screenResolution = (Vector2i){.x = 256, .y = 224};
SDL_Surface* renderSurface;
SDL_Surface* fogSurface;
SDL_Surface* aoSurface;

RayCamera* camera;

void TC_SetupRenderer(Vector2i* mapSizePointer, SDL_Surface* targetSurface) {
    renderMapSize = mapSizePointer;
    renderSurface = targetSurface;
    fogSurface = SDL_CreateRGBSurface(0, 256, 224, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_SetSurfaceBlendMode(fogSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(fogSurface, 255);
    aoSurface = SDL_CreateRGBSurface(0, 256, 224, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_SetSurfaceBlendMode(aoSurface, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod(aoSurface, 255);

    camera = malloc(sizeof(RayCamera));
    camera->cameraPosition = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraDirection = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraPlane = (Vector2){.x = 0.0f, .y = 0.66f};
}

void TC_RenderGeo() {
    // Reset Render
    SDL_FillRect(renderSurface, NULL, SDL_MapRGB(renderSurface->format, FOG_COLOR));
    // Reset Post Processing
    SDL_FillRect(fogSurface, NULL, SDL_MapRGBA(renderSurface->format, FOG_COLOR, 0));
    SDL_FillRect(aoSurface, NULL, SDL_MapRGBA(renderSurface->format, AO_COLOR, 0));

    // Load up the Camera data we need
    float cameraPosX = camera->cameraPosition.x;
    float cameraPosY = camera->cameraPosition.y;
    float cameraDirX = camera->cameraDirection.x;
    float cameraDirY = camera->cameraDirection.y;
    float cameraPlneX = camera->cameraPlane.x;
    float cameraPlneY = camera->cameraPlane.y;
    int screenWidth = screenResolution.x;
    int screenHeight = screenResolution.y;

    // Raycast for each pixel column
    for (int x = 0; x < screenWidth; x++) {
        // Offset of the current column relative to Screen
        float screenX = 2 * x / ((float)screenWidth) - 1;

        // Direction of Ray
        float rayDirX = cameraDirX + cameraPlneX * screenX;
        float rayDirY = cameraDirY + cameraPlneY * screenX;

        // Distance to travel for every scan
        float travelDistX = rayDirX == 0.0f ? 1e30 : fabs(1.0f / rayDirX);
        float travelDistY = rayDirY == 0.0f ? 1e30 : fabs(1.0f / rayDirY);

        // Map distance to travel for every scan
        int travelMapX;
        int travelMapY;

        // Distance travelled so far
        float totalDistX;
        float totalDistY;

        // Current Map coordinate
        int mapCoordX = (int)cameraPosX;
        int mapCoordY = (int)cameraPosY;

        // Setting up X and Y step integers and X and Y side distances
        if (rayDirX < 0.0f) {
            travelMapX = -1;
            totalDistX = (cameraPosX - mapCoordX) * travelDistX;
        } else {
            travelMapX = 1;
            totalDistX = (mapCoordX + 1.0 - cameraPosX) * travelDistX;
        }
        if (rayDirY < 0.0f) {
            travelMapY = -1;
            totalDistY = (cameraPosY - mapCoordY) * travelDistY;
        } else {
            travelMapY = 1;
            totalDistY = (mapCoordY + 1.0 - cameraPosY) * travelDistY;
        }

        // Distance from detected wall to camera
        float wallDepth;

        // Map tile ID found
        int tileId = 0;
        // North/South Plane or East/West Plane
        int facePlane;

        // Digital Differential Analysis
        while (tileId == 0) {
            if (totalDistX < totalDistY) {
                totalDistX += travelDistX;
                mapCoordX += travelMapX;
                // North/South Plane
                facePlane = 0;
            } else {
                totalDistY += travelDistY;
                mapCoordY += travelMapY;
                // East/West Plane
                facePlane = 1;
            }
            tileId = TC_GetMapTile(mapCoordX, mapCoordY);
        }

        // Calculate Wall Depth
        wallDepth = (facePlane == 0) ? (totalDistX - travelDistX) : (totalDistY - travelDistY);

        // Fetch Tile Texture using found Map coords
        SDL_Surface* targetTexture = TC_GetMapTexture(tileId);

        // Calculate Horizontal Texture coord
        float wallX;
        if (facePlane == 0)
            wallX = cameraPosY + wallDepth * rayDirY;
        else
            wallX = cameraPosX + wallDepth * rayDirX;
        wallX -= floor(wallX);

        // Cardinal direction of face
        int face = 0;

        float aoStrength = 1.0;
        int ambientData = TC_GetMapAmbient(mapCoordX, mapCoordY);
        // Modify Texture coord and setup face based on side and Ray Direction
        if(facePlane == 0) {
            if (rayDirX > 0) {
                // North Face
                face = 0;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // North face uses corners in slots 0 and 1
                applyAmbient(2, 1)

                // Reverse texture coord
                wallX = 1.f - wallX;
            } else {
                // South
                face = 2;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // South face uses corners in slots 2 and 3
                applyAmbient(4, 8)
            }
        }
        else if(facePlane == 1) {
            if (rayDirY < 0) {
                // East
                face = 1;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 1 and 2
                applyAmbient(4, 2)

                // Reverse texture coord
                wallX = 1.f - wallX;
            } else {
                // West
                face = 3;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 0 and 3
                applyAmbient(8, 1)
            }
        }
        aoStrength = 1.f - aoStrength;

        // Calculate Horizontal Texture pixel coord
        int textureX = (int)(wallX * (float)targetTexture->w);

        // Height of tile onscreen
        int lineHeight = (int)(screenHeight/wallDepth);

        // Scaling ratio of texture size relative to onscreen height
        float scaleRatio = (float)targetTexture->h/(float)lineHeight;

        // Starting point to draw onscreen
        int drawStart = -lineHeight/2+screenHeight/2 + HEIGHT_OFFSET;
        // Starting point to fetch column from texture
        int fetchStart = 0;
        // Clamp drawing start and push down fetching start
        if (drawStart < 0) {
            fetchStart = -drawStart * scaleRatio;
            drawStart = 0;
        }

        // Ending point to draw onscreen
        int drawEnd = lineHeight/2+screenHeight/2 + HEIGHT_OFFSET;
        // Ending point to fetch column from texture
        int fetchEnd = targetTexture->h-1;
        // Clamp drawing end and push up fetching end
        if (drawEnd >= screenHeight) {
            fetchEnd = targetTexture->h-((drawEnd - screenHeight) * scaleRatio);
            drawEnd = screenHeight-1;
        }

        // Setup screen drawing rect
        SDL_Rect targetRect = (SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart};
        // Blit unshaded texture onto rendering surface
        SDL_BlitScaled(targetTexture, &(SDL_Rect){.x = textureX, .y = fetchStart, .w = 1, .h = fetchEnd-fetchStart}, renderSurface, &targetRect);

        // Calculate strength of environment color based on depth
        float fogStrength = wallDepth / FOG_THICKNESS;
        if (fogStrength < 0.f)
            fogStrength = 0.f;
        else if (fogStrength > 1.f)
            fogStrength = 1.f;

        // Draw Post Processing
        SDL_FillRect(aoSurface, &targetRect, SDL_MapRGBA(aoSurface->format, AO_COLOR, (int)(aoStrength*255)));
        SDL_FillRect(fogSurface, &targetRect, SDL_MapRGBA(fogSurface->format, FOG_COLOR, (int)(fogStrength*255)));
    }
    // Apply Post Processing
    SDL_BlitSurface(aoSurface, NULL, renderSurface, NULL);
    SDL_BlitSurface(fogSurface, NULL, renderSurface, NULL);
}

RayCamera* TC_GetCamera() {
    return camera;
}

void TC_CloseRenderer() {
    free(camera);
}
