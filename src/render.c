#include "../include/render.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

#define FAR_PLANE_DISTANCE 4.75f
#define FOG_START -2.f
#define FOG_END 4.f
#define FOG_COLOR 20, 12, 1
#define AO_COLOR 20, 35, 0
/*#define FOG_START 0.f
#define FOG_END 5.f
#define FOG_COLOR 30, 10, 1
#define AO_COLOR 30, 30, 30*/

#define AO_SHARPNESS 2.f
#define AO_BRIGHTNESS .1f
#define SCAN_DISTANCE 50
#define FLOOR_TILE_COUNT 6

Vector2i* renderMapSize;
SDL_Renderer* mainRenderer;
int rendererFlags = 0;
SDL_Texture* mainRenderTexture;
SDL_Texture* viewRenderTexture;
Vector2i screenSize = (Vector2i){.x = 244, .y = 160};
SDL_Rect viewportRect;

RayCamera* camera;

void TC_SetupRenderer(Vector2i* mapSizePointer, CFW_Window* targetWindow, SDL_Texture* renderTexture) {
    renderMapSize = mapSizePointer;
    mainRenderer = targetWindow->renderer;
    mainRenderTexture = renderTexture;
    viewRenderTexture = SDL_CreateTexture(mainRenderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, screenSize.x, screenSize.y);
    viewportRect = (SDL_Rect){.x = 6, .y = 6, .w = screenSize.x, .h = screenSize.y};

    camera = malloc(sizeof(RayCamera));
    camera->cameraPosition = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraDirection = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraPlane = (Vector2){.x = 0.0f, .y = 0.90f};
    camera->cameraAngle = 0.f;
    camera->horizontalOffset = .2f;
    camera->verticalOffset = 0.f;
}

void TC_RenderFloorCeiling() {
    SDL_SetRenderTarget(mainRenderer, viewRenderTexture);
    float verticalFogOffset = camera->verticalOffset * .5f;

    void** writePixels;
    int* writePitch;

    CFW_Texture* ceilingTexture = TC_GetCeilingTexture();
    CFW_Texture* currentCeiling = ceilingTexture;
    CFW_Texture* floorTexture = TC_GetFloorTexture();
    CFW_Texture* currentFloor = floorTexture;

    Vector2 rayDirection0;
    Vector2 rayDirection1;
    float screenWidthF = (float)screenSize.x;
    float screenHeightF = (float)screenSize.y;
    Vector2 horizontalOffset = (Vector2){.x = camera->cameraPlane.x * camera->horizontalOffset, .y =  camera->cameraPlane.y * camera->horizontalOffset};
    Vector2 cameraPos = (Vector2){.x = camera->cameraPosition.x+horizontalOffset.x, .y = camera->cameraPosition.y+horizontalOffset.y};

    for (int y = screenSize.y/2; y < screenSize.y; y++) {
        rayDirection0 = (Vector2){.x = camera->cameraDirection.x-camera->cameraPlane.x, .y = camera->cameraDirection.y-camera->cameraPlane.y};
        rayDirection1 = (Vector2){.x = camera->cameraDirection.x+camera->cameraPlane.x, .y = camera->cameraDirection.y+camera->cameraPlane.y};

        int currentY = y - screenHeightF / 2;
        float cameraY = .5f * screenHeightF;
        float lineFloorDistance = cameraY/currentY * (1.05f*camera->verticalOffset+1.f);
        float lineCeilDistance = cameraY/currentY * (1.f-1.05f*camera->verticalOffset);

        Vector2 floorDist = (Vector2){.x = lineFloorDistance * (rayDirection1.x-rayDirection0.x) / screenWidthF, .y = lineFloorDistance * (rayDirection1.y-rayDirection0.y) / screenWidthF};
        Vector2 ceilDist = (Vector2){.x = lineCeilDistance * (rayDirection1.x-rayDirection0.x) / screenWidthF, .y = lineCeilDistance * (rayDirection1.y-rayDirection0.y) / screenWidthF};
        Vector2 worldFloorCoord = (Vector2){.x = cameraPos.x + lineFloorDistance * rayDirection0.x, .y = cameraPos.y + lineFloorDistance * rayDirection0.y};
        Vector2 worldCeilCoord = (Vector2){.x = cameraPos.x + lineCeilDistance * rayDirection0.x, .y = cameraPos.y + lineCeilDistance * rayDirection0.y};

        for (int x = 0; x < screenSize.x; x++) {
            Vector2i mapFloorCoord = (Vector2i){.x = (int)worldFloorCoord.x, .y = (int)worldFloorCoord.y};
            int tileFlags = TC_GetMapFlags(mapFloorCoord.x, mapFloorCoord.y);
            bool drawDecal = TC_CHECKIFPAINTFLOOR(tileFlags);
            bool reverseFloorU = (tileFlags & TILEFLAG_MIRRORU) > 0 && drawDecal;
            bool reverseFloorV = (tileFlags & TILEFLAG_MIRRORV) > 0 && drawDecal;
            if (drawDecal)
                currentFloor = TC_GetMapTexture(TC_GetMapTextureID((int)mapFloorCoord.x, (int)mapFloorCoord.y));
            else
                currentFloor = floorTexture;

            Vector2i mapCeilCoord = (Vector2i){.x = (int)worldCeilCoord.x, .y = (int)worldCeilCoord.y};
            tileFlags = TC_GetMapFlags(mapCeilCoord.x, mapCeilCoord.y);
            drawDecal = TC_CHECKIFPAINTCEILING(tileFlags);
            bool reverseCeilingU = (tileFlags & TILEFLAG_MIRRORU) > 0 && drawDecal;
            bool reverseCeilingV = (tileFlags & TILEFLAG_MIRRORV) > 0 && drawDecal;
            if (drawDecal)
                currentCeiling = TC_GetMapTexture(TC_GetMapTextureID((int)mapCeilCoord.x, (int)mapCeilCoord.y));
            else
                currentCeiling = ceilingTexture;

            Vector2i floorUV = (Vector2i){.x = (int)(currentFloor->w * (worldFloorCoord.x-(float)mapFloorCoord.x)) & (currentFloor->w-1), .y = (int)(currentFloor->h * (worldFloorCoord.y-(float)mapFloorCoord.y)) & (currentFloor->h-1)};
            if (reverseFloorU)
                floorUV.x = currentFloor->w-1-floorUV.x;
            if (reverseFloorV)
                floorUV.y = currentFloor->h-1-floorUV.y;
            Vector2i ceilingUV = (Vector2i){.x = (int)(currentCeiling->w * (worldCeilCoord.x-(float)mapCeilCoord.x)) & (currentCeiling->w-1), .y = (int)(currentCeiling->h * (worldCeilCoord.y-(float)mapCeilCoord.y)) & (currentCeiling->h-1)};
            if (reverseCeilingU)
                ceilingUV.x = currentCeiling->w-1-ceilingUV.x;
            if (reverseCeilingV)
                ceilingUV.y = currentCeiling->h-1-ceilingUV.y;

            worldFloorCoord.x += floorDist.x;
            worldFloorCoord.y += floorDist.y;
            worldCeilCoord.x += ceilDist.x;
            worldCeilCoord.y += ceilDist.y;

            SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_NONE);

            Uint32 sampleColor;
            Uint8 redColor;
            Uint8 greenColor;
            Uint8 blueColor;
            Uint8 alphaColor;

            sampleColor = ((Uint32*)currentFloor->surface->pixels)[floorUV.x + floorUV.y*currentFloor->w];
            SDL_GetRGBA(sampleColor, currentFloor->surface->format, &redColor, &greenColor, &blueColor, &alphaColor);

            SDL_SetRenderDrawColor(mainRenderer, redColor, greenColor, blueColor, alphaColor);
            SDL_RenderDrawPoint(mainRenderer, x, y);

            sampleColor = ((Uint32*)currentCeiling->surface->pixels)[ceilingUV.x + ceilingUV.y*currentCeiling->w];
            SDL_GetRGBA(sampleColor, currentCeiling->surface->format, &redColor, &greenColor, &blueColor, &alphaColor);
            SDL_SetRenderDrawColor(mainRenderer, redColor, greenColor, blueColor, alphaColor);
            SDL_RenderDrawPoint(mainRenderer, x, screenSize.y-y-1);
        }

        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
        float floorDistance = (1.f-fabs(((float)y)/((float)screenSize.y)*2.f-1.f)) * FAR_PLANE_DISTANCE;
        float fogStrength = (FOG_END - floorDistance)/(FOG_END-FOG_START);

        float floorFogStrength = fogStrength - verticalFogOffset;
        floorFogStrength = clampFloat(floorFogStrength, 0.f, 1.f);
        floorFogStrength = invertFloat(floorFogStrength);
        float ceilFogStrength = fogStrength + verticalFogOffset;
        ceilFogStrength = clampFloat(ceilFogStrength, 0.f, 1.f);
        ceilFogStrength = invertFloat(ceilFogStrength);

        SDL_SetRenderDrawColor(mainRenderer, FOG_COLOR, (int)(floorFogStrength*255));
        SDL_RenderDrawLine(mainRenderer, 0, y, screenSize.x-1, y);
        SDL_SetRenderDrawColor(mainRenderer, FOG_COLOR, (int)(ceilFogStrength*255));
        SDL_RenderDrawLine(mainRenderer, 0, screenSize.y-y-1, screenSize.x-1, screenSize.y-y-1);
    }
}

void TC_RenderWalls() {
    SDL_SetRenderTarget(mainRenderer, viewRenderTexture);
    bool showFakeWalls = (rendererFlags & TC_RENDER_CHECKCOLLISION) == 0;
    // Load up the Camera data we need
    Vector2 horizontalOffset = (Vector2){.x = camera->cameraPlane.x * camera->horizontalOffset, .y =  camera->cameraPlane.y * camera->horizontalOffset};
    Vector2 cameraPos = (Vector2){.x = camera->cameraPosition.x+horizontalOffset.x, .y = camera->cameraPosition.y+horizontalOffset.y};
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
        Vector2 totalDist = (Vector2){.x = cameraPos.x, .y = cameraPos.y};
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

        // North/South Plane or East/West Plane
        int facePlane;
        // Map tile flags
        int tileFlags = 0;

        if (showFakeWalls) {

            // Digital Differential Analysis
            while (!TC_CHECKIFPAINTWALL(tileFlags) && totalScans < SCAN_DISTANCE) {
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
                tileFlags = TC_GetMapFlags(mapCoord.x, mapCoord.y);
            }

            if (!TC_CHECKIFPAINTWALL(tileFlags))
                continue;
        } else {
            // Map tile collision
            int tileCollide = 0;

            // Digital Differential Analysis
            while ((TC_CHECKIFPAINTWALL(tileFlags) || tileCollide == 0) && totalScans < SCAN_DISTANCE) {
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
                tileFlags = TC_GetMapFlags(mapCoord.x, mapCoord.y);
                tileCollide = TC_GetMapCollision(mapCoord.x, mapCoord.y);
            }

            if (TC_CHECKIFPAINTWALL(tileFlags) || tileCollide == 0)
                continue;
        }

        bool reverseU = (tileFlags & TILEFLAG_MIRRORU) > 0;
        bool reverseV = (tileFlags & TILEFLAG_MIRRORV) > 0;

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
                if (!reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            } else {
                // South
                visibleFace = 2;
                // Check for Ambient Occlusion data, calculate and apply it if present
                // South face uses corners in slots 2 and 3
                applyAmbient(4, 8)
                if (reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            }
        }
        else if(facePlane == 1) {
            if (rayDir.y < 0) {
                // East
                visibleFace = 1;
                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 1 and 2
                applyAmbient(4, 2)
                if (!reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            } else {
                // West
                visibleFace = 3;
                // Check for Ambient Occlusion data, calculate and apply it if present
                // East face uses corners in slots 0 and 3
                applyAmbient(8, 1)
                if (reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            }
        }
        aoStrength = invertFloat(aoStrength);

        // Fetch Tile Texture using found Map coords
        CFW_Texture* targetTexture = TC_GetMapTexture(TC_GetMapTextureID(mapCoord.x, mapCoord.y));
        // Calculate Horizontal Texture pixel coord
        int textureX = (int)(wallX * (float)targetTexture->w);
        // Height of tile onscreen
        int lineHeight = (int)(screenSize.y/wallDepth);
        // Scaling ratio of texture size relative to onscreen height
        float scaleRatio = (float)targetTexture->h/(float)lineHeight;

        // Starting point to draw onscreen
        int drawStart = (int)(-lineHeight/2+screenSize.y/2)+(camera->verticalOffset*lineHeight*.5f);
        // Ending point to draw onscreen
        int drawEnd = (int)(lineHeight/2+screenSize.y/2)+(camera->verticalOffset*lineHeight*.5f);

        // Setup screen drawing rect
        SDL_Rect targetRect = (SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart};
        // Blit unshaded texture onto rendering surface
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_NONE);
        if (reverseV)
            SDL_RenderCopyEx(mainRenderer, targetTexture->texture, &(SDL_Rect){.x = textureX, .y = 0, .w = 1, .h = targetTexture->h}, &targetRect, .0f, NULL, SDL_FLIP_VERTICAL);
        else
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

void TC_RenderViewport() {
    SDL_SetRenderTarget(mainRenderer, mainRenderTexture);
    SDL_RenderCopy(mainRenderer, viewRenderTexture, NULL, &viewportRect);
}

RayCamera* TC_GetCamera() {
    return camera;
}

void TC_CloseRenderer() {
    free(camera);
}
