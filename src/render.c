#include "../include/render.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_surface.h>

#define FOG_THINNING 2.f
#define FOG_COLOR 30, 10, 0
#define AO_COLOR 30, 55, 0
#define AO_SHARPNESS 2.f
#define AO_BRIGHTNESS .1f

Vector2i* renderMapSize;
SDL_Surface* renderSurface;
SDL_Surface* fogSurface;
SDL_Surface* aoSurface;

RayCamera* camera;

void TC_SetupRenderer(Vector2i* mapSizePointer, SDL_Surface* targetSurface) {
    renderMapSize = mapSizePointer;
    renderSurface = targetSurface;
    fogSurface = SDL_CreateRGBSurface(0, renderSurface->w, renderSurface->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_SetSurfaceBlendMode(fogSurface, SDL_BLENDMODE_BLEND);
    aoSurface = SDL_CreateRGBSurface(0, renderSurface->w, renderSurface->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_SetSurfaceBlendMode(aoSurface, SDL_BLENDMODE_BLEND);

    camera = malloc(sizeof(RayCamera));
    camera->cameraPosition = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraDirection = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraPlane = (Vector2){.x = 0.0f, .y = 0.66f};
}

void TC_RenderGeo() {
    // Reset Render
    SDL_FillRect(renderSurface, NULL, SDL_MapRGB(renderSurface->format, 0, 0, 0));
    // Reset Post Processing
    SDL_FillRect(fogSurface, NULL, SDL_MapRGBA(renderSurface->format, FOG_COLOR, 255));
    SDL_FillRect(aoSurface, NULL, SDL_MapRGBA(renderSurface->format, AO_COLOR, 255));

    // Load up the Camera data we need
    Vector2 cameraPos = (Vector2){.x = camera->cameraPosition.x, .y = camera->cameraPosition.y};
    Vector2i screenSize = (Vector2i){.x = renderSurface->w, .y = renderSurface->h};

    // Raycast for each pixel column
    for (int x = 0; x < screenSize.x; x++) {
        // Offset of the current column relative to Screen
        float screenX = 2 * x / ((float)screenSize.x) - 1;
        // Direction of Ray
        Vector2 rayDir = (Vector2){.x = camera->cameraDirection.x + camera->cameraPlane.x * screenX, .y = camera->cameraDirection.y + camera->cameraPlane.y * screenX};
        // Distance to travel for every scan
        Vector2 travelDist = (Vector2){.x = rayDir.x == 0.0f ? 1e30 : fabs(1.0f / rayDir.x), .y = rayDir.y == 0.0f ? 1e30 : fabs(1.0f / rayDir.y)};
        // Map distance to travel for every scan
        Vector2i travelMap = (Vector2i){.x = 0, .y = 0};
        // Distance travelled so far
        Vector2 totalDist = (Vector2){.x = camera->cameraPosition.x, .y = camera->cameraPosition.y};
        // Current Map coordinate
        Vector2i mapCoord = (Vector2i){.x = (int)cameraPos.x, .y = (int)cameraPos.y};

        // Setting up X and Y step integers and X and Y side distances
        if (rayDir.x < 0.0f) {
            travelMap.x = -1;
            totalDist.x = (cameraPos.x - mapCoord.x) * travelDist.x;
        } else {
            travelMap.x = 1;
            totalDist.x = (mapCoord.x + 1.0 - cameraPos.x) * travelDist.x;
        }
        if (rayDir.y < 0.0f) {
            travelMap.y = -1;
            totalDist.y = (cameraPos.y - mapCoord.y) * travelDist.y;
        } else {
            travelMap.y = 1;
            totalDist.y = (mapCoord.y + 1.0 - cameraPos.y) * travelDist.y;
        }

        // Map tile ID found
        int tileId = 0;
        // North/South Plane or East/West Plane
        int facePlane;

        // Digital Differential Analysis
        while (tileId == 0) {
            if (totalDist.x < totalDist.y) {
                totalDist.x += travelDist.x;
                mapCoord.x += travelMap.x;
                // North/South Plane
                facePlane = 0;
            } else {
                totalDist.y += travelDist.y;
                mapCoord.y += travelMap.y;
                // East/West Plane
                facePlane = 1;
            }
            tileId = TC_GetMapTile(mapCoord.x, mapCoord.y);
        }

        // Distance from detected wall to camera
        float wallDepth = (facePlane == 0) ? (totalDist.x - travelDist.x) : (totalDist.y - travelDist.y);
        // Fetch Tile Texture using found Map coords
        SDL_Surface* targetTexture = TC_GetMapTexture(tileId);

        // Calculate Horizontal Texture coord
        float wallX;
        if (facePlane == 0)
            wallX = cameraPos.y + wallDepth * rayDir.y;
        else
            wallX = cameraPos.x + wallDepth * rayDir.x;
        wallX -= floor(wallX);

        // Cardinal direction of face
        int visibleFace = 0;

        float aoStrength = 1.0;
        int ambientData = TC_GetMapAmbient(mapCoord.x, mapCoord.y);
        // Modify Texture coord and setup face based on side and Ray Direction
        if(facePlane == 0) {
            if (rayDir.x > 0) {
                // North Face
                visibleFace = 0;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // North face uses corners in slots 0 and 1
                applyAmbient(2, 1)

                // Reverse texture coord
                wallX = 1.f - wallX;
            } else {
                // South
                visibleFace = 2;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // South face uses corners in slots 2 and 3
                applyAmbient(4, 8)
            }
        }
        else if(facePlane == 1) {
            if (rayDir.y < 0) {
                // East
                visibleFace = 1;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 1 and 2
                applyAmbient(4, 2)

                // Reverse texture coord
                wallX = 1.f - wallX;
            } else {
                // West
                visibleFace = 3;

                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 0 and 3
                applyAmbient(8, 1)
            }
        }
        aoStrength = 1.f - aoStrength;

        // Calculate Horizontal Texture pixel coord
        int textureX = (int)(wallX * (float)targetTexture->w);

        // Height of tile onscreen
        int lineHeight = (int)(screenSize.y/wallDepth);

        // Scaling ratio of texture size relative to onscreen height
        float scaleRatio = (float)targetTexture->h/(float)lineHeight;

        // Starting point to draw onscreen
        int drawStart = -lineHeight/2+screenSize.y/2;
        // Starting point to fetch column from texture
        int fetchStart = 0;
        // Clamp drawing start and push down fetching start
        if (drawStart < 0) {
            fetchStart = -drawStart * scaleRatio;
            drawStart = 0;
        }

        // Ending point to draw onscreen
        int drawEnd = lineHeight/2+screenSize.y/2;
        // Ending point to fetch column from texture
        int fetchEnd = targetTexture->h-1;
        // Clamp drawing end and push up fetching end
        if (drawEnd >= screenSize.y) {
            fetchEnd = targetTexture->h-((drawEnd - screenSize.y) * scaleRatio);
            drawEnd = screenSize.y-1;
        }

        // Setup screen drawing rect
        SDL_Rect targetRect = (SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart};
        // Blit unshaded texture onto rendering surface
        SDL_BlitScaled(targetTexture, &(SDL_Rect){.x = textureX, .y = fetchStart, .w = 1, .h = fetchEnd-fetchStart}, renderSurface, &targetRect);

        // Calculate strength of environment color based on depth
        float fogStrength = wallDepth / FOG_THINNING;
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
