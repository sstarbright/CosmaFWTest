#include "../include/render.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

#define FAR_PLANE_DISTANCE 4.75f
#define SCAN_DISTANCE 50

Vector2i* renderMapSize;
SDL_Renderer* mainRenderer;
int rendererFlags = 0;
SDL_Texture* mainRenderTexture;
SDL_Texture* viewRenderTexture;
Vector2i screenSize = (Vector2i){.x = 244, .y = 160};
SDL_Rect viewportRect;

RayCamera* camera;

// Fog Rendering Data
float fog_start = -2.f;
float fog_end = 4.f;
CFW_Color fog_color = (CFW_Color){.r = 20, .g = 12, .b = 1};

// AO Rendering Data
float ao_sharpness = 2.f;
float ao_transparency = .1f;
CFW_Color ao_color = (CFW_Color){.r = 20, .g = 35, .b = 0};

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
            bool swapFloorUV = (tileFlags & TILEFLAG_SWAPUV) > 0 && drawDecal;

            float floorU = (worldFloorCoord.x-(float)mapFloorCoord.x);
            float floorV = (worldFloorCoord.y-(float)mapFloorCoord.y);

            float floorNorth = 0.f;
            float floorSouth = 0.f;
            float floorWest = 0.f;
            float floorEast = 0.f;
            SETUP_PADDING(floorNorth, floorSouth, floorWest, floorEast)

            floorNorth += .5f;
            floorSouth += .5f;
            floorWest += .5f;
            floorEast += .5f;

            if (drawDecal && floorU <= floorNorth && floorU >= floorSouth && floorV <= floorWest && floorV >= floorEast)
                currentFloor = TC_GetMapTexture(TC_GetMapTextureID((int)mapFloorCoord.x, (int)mapFloorCoord.y));
            else
                currentFloor = floorTexture;

            Vector2i floorUV = (Vector2i){.x = (int)(currentFloor->w * floorU) & (currentFloor->w-1), .y = (int)(currentFloor->h * floorV) & (currentFloor->h-1)};

            Vector2i mapCeilCoord = (Vector2i){.x = (int)worldCeilCoord.x, .y = (int)worldCeilCoord.y};
            tileFlags = TC_GetMapFlags(mapCeilCoord.x, mapCeilCoord.y);
            drawDecal = TC_CHECKIFPAINTCEILING(tileFlags);
            bool reverseCeilingU = (tileFlags & TILEFLAG_MIRRORU) > 0 && drawDecal;
            bool reverseCeilingV = (tileFlags & TILEFLAG_MIRRORV) > 0 && drawDecal;
            bool swapCeilingUV = (tileFlags & TILEFLAG_SWAPUV) > 0 && drawDecal;

            float ceilingU = (worldCeilCoord.x-(float)mapCeilCoord.x);
            float ceilingV = (worldCeilCoord.y-(float)mapCeilCoord.y);

            float ceilingNorth = 0.f;
            float ceilingSouth = 0.f;
            float ceilingWest = 0.f;
            float ceilingEast = 0.f;
            SETUP_PADDING(ceilingNorth, ceilingSouth, ceilingWest, ceilingEast)

            ceilingNorth += .5f;
            ceilingSouth += .5f;
            ceilingWest += .5f;
            ceilingEast += .5f;

            if (drawDecal && ceilingU <= ceilingNorth && ceilingU >= ceilingSouth && ceilingV <= ceilingWest && ceilingV >= ceilingEast)
                currentCeiling = TC_GetMapTexture(TC_GetMapTextureID((int)mapCeilCoord.x, (int)mapCeilCoord.y));
            else
                currentCeiling = ceilingTexture;
            Vector2i ceilingUV = (Vector2i){.x = (int)(currentCeiling->w * (worldCeilCoord.x-(float)mapCeilCoord.x)) & (currentCeiling->w-1), .y = (int)(currentCeiling->h * (worldCeilCoord.y-(float)mapCeilCoord.y)) & (currentCeiling->h-1)};

            if (reverseFloorU)
                floorUV.x = currentFloor->w-1-floorUV.x;
            if (reverseFloorV)
                floorUV.y = currentFloor->h-1-floorUV.y;
            if (swapFloorUV) {
                float oldValue = floorUV.x;
                floorUV.x = floorUV.y;
                floorUV.y = oldValue;
            }

            if (reverseCeilingU)
                ceilingUV.x = currentCeiling->w-1-ceilingUV.x;
            if (reverseCeilingV)
                ceilingUV.y = currentCeiling->h-1-ceilingUV.y;
            if (swapCeilingUV) {
                float oldValue = ceilingUV.x;
                ceilingUV.x = ceilingUV.y;
                ceilingUV.y = oldValue;
            }

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
        float fogStrength = (fog_end - floorDistance)/(fog_end-fog_start);

        float floorFogStrength = fogStrength - verticalFogOffset;
        floorFogStrength = clampFloat(floorFogStrength, 0.f, 1.f);
        floorFogStrength = invertFloat(floorFogStrength);
        float ceilFogStrength = fogStrength + verticalFogOffset;
        ceilFogStrength = clampFloat(ceilFogStrength, 0.f, 1.f);
        ceilFogStrength = invertFloat(ceilFogStrength);

        SDL_SetRenderDrawColor(mainRenderer, fog_color.r, fog_color.g, fog_color.b, (int)(floorFogStrength*255));
        SDL_RenderDrawLine(mainRenderer, 0, y, screenSize.x-1, y);
        SDL_SetRenderDrawColor(mainRenderer, fog_color.r, fog_color.g, fog_color.b, (int)(ceilFogStrength*255));
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

        float rayLength = sqrtf(rayDir.x*rayDir.x+rayDir.y*rayDir.y);
        Vector2 rayNorm = (Vector2){.x = rayDir.x/rayLength, .y = rayDir.y/rayLength};

        Line2 rayLine = (Line2){.start = (Vector2){.x = cameraPos.x, .y = cameraPos.y}, .end = (Vector2){.x = cameraPos.x+rayDir.x*SCAN_DISTANCE, .y = cameraPos.y+rayDir.y*SCAN_DISTANCE}};

        Line2 wallLine = (Line2){.start = (Vector2){.x = 0.f, .y = 0.f}, .end = (Vector2){.x = 0.f, .y = 0.f}};

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
        bool wallFound = false;
        bool isFullWall = false;
        float travelAmount = 0.f;

        float northWallOffset = .5f;
        float southWallOffset = -.5f;
        float westWallOffset = .5f;
        float eastWallOffset = -.5f;

        if (showFakeWalls) {
            // Digital Differential Analysis
            while (!wallFound && totalScans < SCAN_DISTANCE) {
                totalScans += 1;
                if (totalDist.x < totalDist.y) {
                    mapCoord.x += travelMap.x;
                    totalDist.x += travelDist.x;

                    tileFlags = TC_GetMapFlags(mapCoord.x, mapCoord.y);
                    if (TC_CHECKIFPAINTWALL(tileFlags)) {
                        SETUP_PADDING(northWallOffset, southWallOffset, westWallOffset, eastWallOffset)
                        isFullWall = (tileFlags & TILEFLAG_PADNORTH) == 0 && (tileFlags & TILEFLAG_PADSOUTH) == 0 && (tileFlags & TILEFLAG_PADEAST) == 0 && (tileFlags & TILEFLAG_PADWEST) == 0;
                        if (!isFullWall) {
                            SETUP_X_WALL();

                            float wallPosition = (cameraPos.y + (totalDist.x - travelDist.x) * rayDir.y);
                            Vector2 hitPosition = (Vector2){.x = mapCoord.x+(rayDir.x > 0 ? -.5f : .5f), .y = wallPosition-.5f};
                            SETUP_RAY_LINE();

                            if (TC_CheckLineIntersect(rayLine, wallLine)) {
                                wallFound = true;

                                // Check if Wall Position is past the offset points for this side. If so, set face plane to North/South and adjust totalDist for that
                                totalDist.x += travelDist.x * (.5f-fabs(rayDir.x > 0 ? southWallOffset : northWallOffset));

                                // North/South Plane
                                facePlane = 0;
                            } else {
                                SETUP_Y_WALL();

                                if (TC_CheckLineIntersect(rayLine, wallLine)) {
                                    wallFound = true;

                                    totalDist.y += travelDist.y * (.5f-fabs(rayDir.y > 0 ? eastWallOffset : westWallOffset));

                                    facePlane = 1;
                                }
                            }
                        } else {
                            wallFound = true;
                            facePlane = 0;
                        }
                    }
                } else {
                    mapCoord.y += travelMap.y;
                    totalDist.y += travelDist.y;

                    tileFlags = TC_GetMapFlags(mapCoord.x, mapCoord.y);
                    if (TC_CHECKIFPAINTWALL(tileFlags)) {
                        SETUP_PADDING(northWallOffset, southWallOffset, westWallOffset, eastWallOffset)
                        isFullWall = (tileFlags & TILEFLAG_PADNORTH) == 0 && (tileFlags & TILEFLAG_PADSOUTH) == 0 && (tileFlags & TILEFLAG_PADEAST) == 0 && (tileFlags & TILEFLAG_PADWEST) == 0;
                        if (!isFullWall) {
                            SETUP_Y_WALL();

                            float wallPosition = (cameraPos.x + (totalDist.y - travelDist.y) * rayDir.x);
                            Vector2 hitPosition = (Vector2){.x = wallPosition-.5f, .y = mapCoord.y+(rayDir.y > 0 ? -.5f : .5f)};
                            SETUP_RAY_LINE();

                            if (TC_CheckLineIntersect(rayLine, wallLine)) {
                                wallFound = true;

                                totalDist.y += travelDist.y * (.5f-fabs(rayDir.y > 0 ? eastWallOffset : westWallOffset));

                                // East/West Plane
                                facePlane = 1;
                            } else {
                                SETUP_X_WALL();

                                if (TC_CheckLineIntersect(rayLine, wallLine)) {
                                    wallFound = true;

                                    totalDist.x += travelDist.x * (.5f-fabs(rayDir.x > 0 ? southWallOffset : northWallOffset));

                                    facePlane = 0;
                                }
                            }
                        } else {
                            wallFound = true;
                            facePlane = 1;
                        }
                    }
                }
            }

            if (!wallFound)
                continue;
        } else {
            // Map tile collision
            int tileCollide = 0;
        }

        bool reverseU = (tileFlags & TILEFLAG_MIRRORU) > 0;
        bool reverseV = (tileFlags & TILEFLAG_MIRRORV) > 0;
        bool swapUV = (tileFlags & TILEFLAG_SWAPUV) > 0;

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

        float wallClamp;
        // Modify Texture coord and setup face based on side and Ray Direction
        if(facePlane == 0) {
            if (rayDir.x > 0) {
                // North Face
                visibleFace = 0;
                if (!reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            } else {
                // South
                visibleFace = 2;
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
                if (!reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            } else {
                // West
                visibleFace = 3;
                if (reverseU) {
                    // Reverse texture coord
                    wallX = invertFloat(wallX);
                }
            }
        }

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
        SDL_Rect sourceRect = (SDL_Rect){.x = textureX, .y = 0, .w = 1, .h = targetTexture->h};
        SDL_Rect targetRect = (SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart};
        int mirrorFlag = reverseV ? SDL_FLIP_VERTICAL : 0;
        float rotationAngle = .0f;
        if (swapUV) {
            int swapBuffer = sourceRect.x;
            sourceRect.x = sourceRect.y;
            sourceRect.y = swapBuffer;
            swapBuffer = sourceRect.w;
            sourceRect.w = sourceRect.h;
            sourceRect.h = swapBuffer;

            targetRect.x += 1;
            swapBuffer = targetRect.w;
            targetRect.w = targetRect.h;
            targetRect.h = swapBuffer;
            rotationAngle = 90.f;
        }
        // Blit unshaded texture onto rendering surface
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_NONE);
        if (reverseV || swapUV)
            SDL_RenderCopyEx(mainRenderer, targetTexture->texture, &sourceRect, &targetRect, rotationAngle, &(SDL_Point){.x = 0, .y = 0}, mirrorFlag);
        else
            SDL_RenderCopy(mainRenderer, targetTexture->texture, &sourceRect, &targetRect);

        // Calculate strength of environment color based on depth
        float fogStrength = (fog_end - wallDepth)/(fog_end-fog_start);
        fogStrength = invertFloat(fogStrength);
        fogStrength = clampFloat(fogStrength, 0.f, 1.f);

        // Draw FOG
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mainRenderer, fog_color.r, fog_color.g, fog_color.b, (int)(fogStrength*255));
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
