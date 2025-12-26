#include "../include/render.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

#define FLOOR_COLOR 64, 50, 0
#define CEILING_COLOR 64, 64, 64
#define FAR_PLANE_DISTANCE 4.75f
#define FOG_START -1.5f
#define FOG_END 3.f
#define FOG_COLOR 20, 12, 1
#define AO_COLOR 40, 55, 0
#define AO_SHARPNESS 2.f
#define AO_BRIGHTNESS .1f
#define SCAN_DISTANCE 50
#define FLOOR_TILE_COUNT 6

Vector2i* renderMapSize;
SDL_Renderer* mainRenderer;
SDL_Texture* mainRenderTexture;
Vector2i screenSize;

RayCamera* camera;

void TC_SetupRenderer(Vector2i* mapSizePointer, CFW_Window* targetWindow, SDL_Texture* renderTexture) {
    renderMapSize = mapSizePointer;
    mainRenderer = targetWindow->renderer;
    mainRenderTexture = renderTexture;

    SDL_SetRenderTarget(mainRenderer, mainRenderTexture);
    SDL_RenderGetLogicalSize(mainRenderer, &(screenSize.x), &(screenSize.y));

    camera = malloc(sizeof(RayCamera));
    camera->cameraPosition = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraDirection = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraPlane = (Vector2){.x = 0.0f, .y = 0.66f};
    camera->cameraAngle = 0.f;
}

void TC_RenderFloorCeiling() {
    void** writePixels;
    int* writePitch;

    CFW_Texture* ceilingTexture = TC_GetCeilingTexture();
    CFW_Texture* floorTexture = TC_GetFloorTexture();

    Vector2 rayDirection0;
    Vector2 rayDirection1;
    float screenWidthF = (float)screenSize.x;
    float screenHeightF = (float)screenSize.y;

    for (int y = screenSize.y/2; y < screenSize.y; y++) {
        rayDirection0 = (Vector2){.x = camera->cameraDirection.x-camera->cameraPlane.x, .y = camera->cameraDirection.y-camera->cameraPlane.y};
        rayDirection1 = (Vector2){.x = camera->cameraDirection.x+camera->cameraPlane.x, .y = camera->cameraDirection.y+camera->cameraPlane.y};

        int currentY = y - screenHeightF / 2;
        float cameraY = .5f * screenHeightF;
        float lineDistance = cameraY/currentY;

        Vector2 floorDist = (Vector2){.x = lineDistance * (rayDirection1.x-rayDirection0.x) / screenWidthF, .y = lineDistance * (rayDirection1.y-rayDirection0.y) / screenWidthF};
        Vector2 worldCoord = (Vector2){.x = camera->cameraPosition.x + lineDistance * rayDirection0.x, .y = camera->cameraPosition.y + lineDistance * rayDirection0.y};

        for (int x = 0; x < screenSize.x; x++) {
            Vector2i mapCoord = (Vector2i){.x = (int)worldCoord.x, .y = (int)worldCoord.y};
            Vector2i floorUV = (Vector2i){.x = (int)(floorTexture->w * (worldCoord.x-(float)mapCoord.x)) & (floorTexture->w-1), .y = (int)(floorTexture->h * (worldCoord.y-(float)mapCoord.y)) & (floorTexture->h-1)};
            Vector2i ceilingUV = (Vector2i){.x = (int)(ceilingTexture->w * (worldCoord.x-(float)mapCoord.x)) & (ceilingTexture->w-1), .y = (int)(ceilingTexture->h * (worldCoord.y-(float)mapCoord.y)) & (ceilingTexture->h-1)};

            worldCoord.x += floorDist.x;
            worldCoord.y += floorDist.y;

            SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_NONE);

            Uint32 sampleColor;
            Uint8 redColor;
            Uint8 greenColor;
            Uint8 blueColor;
            Uint8 alphaColor;

            sampleColor = ((Uint32*)floorTexture->surface->pixels)[floorUV.x + floorUV.y*floorTexture->w];
            SDL_GetRGBA(sampleColor, floorTexture->surface->format, &redColor, &greenColor, &blueColor, &alphaColor);

            SDL_SetRenderDrawColor(mainRenderer, redColor, greenColor, blueColor, alphaColor);
            SDL_RenderDrawPoint(mainRenderer, x, y);

            sampleColor = ((Uint32*)ceilingTexture->surface->pixels)[ceilingUV.x + ceilingUV.y*ceilingTexture->w];
            SDL_GetRGBA(sampleColor, ceilingTexture->surface->format, &redColor, &greenColor, &blueColor, &alphaColor);
            SDL_SetRenderDrawColor(mainRenderer, redColor, greenColor, blueColor, alphaColor);
            SDL_RenderDrawPoint(mainRenderer, x, screenSize.y-y-1);
        }

        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
        float floorDistance = (1.f-fabs(((float)y)/((float)screenSize.y)*2.f-1.f)) * FAR_PLANE_DISTANCE;
        float fogStrength = (FOG_END - floorDistance)/(FOG_END-FOG_START);
        fogStrength = clampFloat(fogStrength, 0.f, 1.f);
        fogStrength = invertFloat(fogStrength);
        SDL_SetRenderDrawColor(mainRenderer, FOG_COLOR, (int)(fogStrength*255));
        SDL_RenderDrawLine(mainRenderer, 0, y, screenSize.x-1, y);
        SDL_RenderDrawLine(mainRenderer, 0, screenSize.y-y-1, screenSize.x-1, screenSize.y-y-1);
    }
}

void TC_RenderWalls() {
    // Load up the Camera data we need
    Vector2 cameraPos = (Vector2){.x = camera->cameraPosition.x, .y = camera->cameraPosition.y};
    Vector2i mapSize = (Vector2i){.x = renderMapSize->x, .y = renderMapSize->y};

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
        int totalScans = 0;

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
        while (tileId == 0 && totalScans < SCAN_DISTANCE) {
            totalScans += 1;
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

        if (tileId == 0)
            continue;

        // Distance from detected wall to camera
        float wallDepth = (facePlane == 0) ? (totalDist.x - travelDist.x) : (totalDist.y - travelDist.y);

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
        float wallClamp;
        // Modify Texture coord and setup face based on side and Ray Direction
        if(facePlane == 0) {
            if (rayDir.x > 0) {
                // North Face
                visibleFace = 0;
                // Check for Ambient Occlusion data, calculate and apply it if present
                // North face uses corners in slots 0 and 1
                applyAmbient(2, 1)
                // Reverse texture coord
                wallX = invertFloat(wallX);
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
                wallX = invertFloat(wallX);
            } else {
                // West
                visibleFace = 3;
                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 0 and 3
                applyAmbient(8, 1)
            }
        }
        aoStrength = invertFloat(aoStrength);

        // Fetch Tile Texture using found Map coords
        CFW_Texture* targetTexture = TC_GetMapTexture(tileId);
        // Calculate Horizontal Texture pixel coord
        int textureX = (int)(wallX * (float)targetTexture->w);
        // Height of tile onscreen
        int lineHeight = (int)(screenSize.y/wallDepth);
        // Scaling ratio of texture size relative to onscreen height
        float scaleRatio = (float)targetTexture->h/(float)lineHeight;

        // Starting point to draw onscreen
        int drawStart = -lineHeight/2+screenSize.y/2;
        // Ending point to draw onscreen
        int drawEnd = lineHeight/2+screenSize.y/2;

        // Setup screen drawing rect
        SDL_Rect targetRect = (SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart};
        // Blit unshaded texture onto rendering surface
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(mainRenderer, targetTexture->texture, &(SDL_Rect){.x = textureX, .y = 0, .w = 1, .h = targetTexture->h}, &targetRect);

        // Calculate strength of environment color based on depth
        float fogStrength = (FOG_END - wallDepth)/(FOG_END-FOG_START);
        fogStrength = invertFloat(fogStrength);
        fogStrength = clampFloat(fogStrength, 0.f, 1.f);

        // Draw FOG and AO
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mainRenderer, AO_COLOR, (int)(aoStrength*255));
        SDL_RenderDrawLine(mainRenderer, x, drawStart, x, drawEnd-1);
        SDL_SetRenderDrawColor(mainRenderer, FOG_COLOR, (int)(fogStrength*255));
        SDL_RenderDrawLine(mainRenderer, x, drawStart, x, drawEnd-1);
    }
}

RayCamera* TC_GetCamera() {
    return camera;
}

void TC_CloseRenderer() {
    free(camera);
}
