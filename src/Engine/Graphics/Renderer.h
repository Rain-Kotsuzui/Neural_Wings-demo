#pragma once
#include "RenderView.h"
#include "raylib.h"
#include "Camera/mCamera.h"
#include "RenderMaterial.h"
#include "Engine/Math/Math.h"
#include "PostProcessPass.h"

#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class GameWorld;
class CameraManager;
class Mesh;
class Model;

class Renderer
{
public:
    Renderer() = default;

    void RenderScene(GameWorld &world, CameraManager &cameraManager);

    bool LoadViewConfig(const std::string &configPath, GameWorld &gameWorld);

private:
    void ClearRenderViews();
    void AddRenderView(const RenderView &view);

    RenderView ParseViews(const json &data);
    void ParsePostProcessPasses(const json &data, GameWorld &gameWorld);

    void AddPostProcessPass(const PostProcessPass &pass);
    void SetUpRTPool(const std::vector<std::string> &names, int width, int height);
    void UnloadRTPool();

    std::vector<RenderView> m_renderViews;

    void PostProcess(RenderTexture2D &itScene, GameWorld &gameWorld);
    void RawRenderScene(GameWorld &gameWorld, CameraManager &cameraManager);
    void RenderSinglePass(const Mesh &mesh, const Model &model, const int &meshIdx, const RenderMaterial &pass, const Matrix4f &MVP, const Matrix4f &M, const mCamera &camera, GameWorld &gameWorld);
    void DrawWorldObjects(GameWorld &gameWorld, Camera3D &rawCamera, mCamera &camera, float aspect);

    // RT 池
    std::unordered_map<std::string, RenderTexture2D> m_RTPool;
    std::vector<PostProcessPass> m_postProcessPasses;
};