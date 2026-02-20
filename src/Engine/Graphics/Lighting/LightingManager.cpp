#include "LightingManager.h"
#include "Engine/Core/Components/Components.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Graphics/Renderer.h"

LightingManager::~LightingManager()
{
    for (auto &rt : m_shadowMaps)
    {
        if (rt.id > 0)
            UnloadRenderTexture(rt);
    }
}
void LightingManager::Update(GameWorld &world)
{
    m_activeLights.clear();
    m_activeCasters.clear();

    auto entities = world.GetEntitiesWith<LightComponent, TransformComponent>();
    int shadowCount = 0;
    for (auto *entity : entities)
    {
        if (m_activeLights.size() >= MAX_LIGHTS)
            break;

        auto &light = entity->GetComponent<LightComponent>();
        auto &tf = entity->GetComponent<TransformComponent>();

        LightInfo info;
        info.data = &light;
        info.worldPosition = tf.GetWorldPosition();
        if (light.type == LightType::Directional)
        {
            info.worldDirection = tf.GetWorldRotation() * (light.direction).Normalized();
        }
        else
        {
            info.worldDirection = light.direction;
        }

        if (light.castShadows && light.type == LightType::Directional && shadowCount < MAX_SHADOW_CASTERS)
        {

            ShadowCasterData caster;
            caster.textureIndex = shadowCount;
            caster.light = &light;
            caster.lightDir = info.worldDirection;

            m_activeCasters.push_back(caster);

            info.shadowIndex = shadowCount;
            light.shadowIndex = shadowCount;

            shadowCount++;
        }
        else
        {
            info.shadowIndex = -1;
        }

        m_activeLights.push_back(info);
    }
}
void LightingManager::UploadToShader(std::shared_ptr<ShaderWrapper> shader, const Vector3f &viewPos, int texUnit)
{
    if (!shader || !shader->IsValid())
        return;
    shader->SetInt("lightCounts", (int)m_activeLights.size());
    shader->SetVec3("viewPos", viewPos);

    for (int i = 0; i < m_activeLights.size(); ++i)
    {
        std::string base = "lights[" + std::to_string(i) + "]";
        const auto &info = m_activeLights[i];

        shader->SetInt(base + ".type", (int)info.data->type);
        shader->SetVec3(base + ".position", info.worldPosition);
        shader->SetVec3(base + ".direction", info.worldDirection);
        shader->SetVec3(base + ".color", info.data->color / 255.0f);
        shader->SetFloat(base + ".intensity", info.data->intensity);
        shader->SetFloat(base + ".range", info.data->range);
        shader->SetInt(base + ".shadowIndex", info.shadowIndex);
        if (info.shadowIndex >= 0)
        {
            shader->SetFloat(base + ".shadowBias", info.data->shadowBias);
        }
    }

    // 避开材质贴图
    int shadowUnitBase = texUnit + 1;
    for (int i = 0; i < m_activeCasters.size(); ++i)
    {
        std::string baseVP = "lightVPs[" + std::to_string(i) + "]";
        std::string baseMap = "shadowMaps[" + std::to_string(i) + "]";

        shader->SetMat4(baseVP, m_activeCasters[i].lightVP);
        shader->SetTexture(baseMap, m_shadowMaps[m_activeCasters[i].textureIndex].depth, shadowUnitBase + i);
    }
}

void LightingManager::InitShadowMaps(int width, int height, ResourceManager &rm)
{
    m_shadowMaps.resize(MAX_SHADOW_CASTERS);
    for (int i = 0; i < MAX_SHADOW_CASTERS; ++i)
    {
        m_shadowMaps[i] = Renderer::LoadRT(width, height, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    }
    m_depthShader = rm.GetShader("assets/shaders/lighting/depth.vs", "assets/shaders/lighting/depth.fs");
}

void LightingManager::RenderShadowMaps(GameWorld &world, const Vector3f &centerPos)
{
    if (!m_depthShader)
        return;

    rlEnableBackfaceCulling();
    rlSetCullFace(RL_CULL_FACE_FRONT);
    rlEnableDepthTest();
    rlEnableDepthMask();

    auto renderables = world.GetEntitiesWith<RenderComponent, TransformComponent>();

    for (auto &caster : m_activeCasters)
    { // 目前只处理 Directional
        caster.lightVP = CalculateDirectionalLightVP(caster.lightDir, centerPos);

        BeginTextureMode(m_shadowMaps[caster.textureIndex]);
        ClearBackground(WHITE);

        m_depthShader->Begin();
        m_depthShader->SetMat4("lightVP", caster.lightVP);
        for (auto *obj : renderables)
        {

            auto &render = obj->GetComponent<RenderComponent>();
            auto &tf = obj->GetComponent<TransformComponent>();
            if (!render.castShadows)
                continue;
            Matrix4f modelMat = tf.GetWorldMatrix();
            m_depthShader->SetMat4("model", modelMat);

            for (int i = 0; i < render.model.meshCount; i++)
            {

                Material tempRaylibMaterial = render.model.materials[i];

                tempRaylibMaterial.shader = m_depthShader->GetShader();

                DrawMesh(render.model.meshes[i], tempRaylibMaterial, modelMat);
            }
        }
        m_depthShader->End();
        EndTextureMode();
    }

    rlSetCullFace(RL_CULL_FACE_BACK);
}
Matrix4f LightingManager::CalculateDirectionalLightVP(const Vector3f &lightDir, const Vector3f &centerPos)
{
    Vector3f lightPos = centerPos - lightDir * 40.0f;
    Vector3f up = {0.0f, 1.0f, 0.0f};
    Vector3f dirNormalized = Vector3Normalize(lightDir);

    if (fabs(up * dirNormalized) > 0.99f)
    {
        up = {0.0f, 0.0f, 1.0f};
    }
    Matrix matView = MatrixLookAt(lightPos, centerPos, up);

    float size = 100.0f;
    float nearPlane = 0.1f;
    float farPlane = 500.0f;
    Matrix matProj = MatrixOrtho(-size, size, -size, size, nearPlane, farPlane);

    return Matrix4f(MatrixMultiply(matView, matProj));
}

RenderTexture2D *LightingManager::GetShadowMap(int index)
{
    if (index >= 0 && index < m_shadowMaps.size())
        return &m_shadowMaps[index];
}