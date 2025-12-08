#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// ------------------------
const int screenWidth = 800;
const int screenHeight = 450;
Vector2 ballPosition = {0};

void UpdateDrawFrame(void)
{
    if (IsKeyDown(KEY_RIGHT))
        ballPosition.x += 2.0f;
    if (IsKeyDown(KEY_LEFT))
        ballPosition.x -= 2.0f;
    if (IsKeyDown(KEY_UP))
        ballPosition.y -= 2.0f;
    if (IsKeyDown(KEY_DOWN))
        ballPosition.y += 2.0f;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Press To Move", 10, 10, 20, DARKGRAY);
    DrawCircleV(ballPosition, 50, MAROON);
    EndDrawing();
}

#if defined(PLATFORM_WEB)
void web()
{
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
}
#else if
void desktop()
{
    SetTargetFPS(60);
    while (!WindowShouldClose())
        UpdateDrawFrame();

    CloseWindow();
}
#endif

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Test");

    ballPosition.x = (float)screenWidth / 2;
    ballPosition.y = (float)screenHeight / 2;

#if defined(PLATFORM_WEB)
    web();
#else
    desktop();
#endif

    return 0;
}