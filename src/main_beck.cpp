#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Camera3D camera;
Model model;

void InitGame(void)
{
    InitWindow(800, 600, "Raylib OBJ Loading Example");

    camera.position = Vector3{10.0f, 10.0f, 10.0f}; // 相机位置
    camera.target = Vector3{0.0f, 0.0f, 0.0f};      // 相机看向的目标
    camera.up = Vector3{0.0f, 1.0f, 0.0f};          // 相机向上的向量
    camera.fovy = 45.0f;                              // 视野角度
    camera.projection = CAMERA_PERSPECTIVE;           // 透视投影

    model = LoadModel("resources/test.obj");
}

void UpdateDrawFrame(void)
{
    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawModelEx(model, 
        Vector3{0.0f, 0.0f, 0.0f},  // 位置
        Vector3{1.0f, 0.0f, 0.0f},  // 旋转轴
        -90.0f,                      // 旋转角度
        Vector3{1.0f, 1.0f, 1.0f},  // 缩放
        WHITE);                     // 颜色
    DrawGrid(20, 1.0f);
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void UnloadGame(void)
{
    UnloadModel(model);
    CloseWindow();
}

#if defined(PLATFORM_WEB)
void web()
{
    // Web 平台主循环
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
}
#else
void desktop()
{
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
}
#endif

int main_beck(void)
{
    InitGame();

#if defined(PLATFORM_WEB)
    web();
#else
    desktop();
#endif

    UnloadGame();

    return 0;
}