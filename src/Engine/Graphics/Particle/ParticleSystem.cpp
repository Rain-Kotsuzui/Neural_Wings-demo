#include "ParticleSystem.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Core/Components/ParticleEmitterComponent.h"
#include "Engine/Core/Components/TransformComponent.h"
#include "rlgl.h"

ParticleSystem::ParticleSystem()
{
    m_TFBManager = std::make_unique<TFBManager>();
}
ParticleSystem::~ParticleSystem()
{
    m_emitterBuffers.clear();
};

GPUParticleBuffer *ParticleSystem::GetOrCreateBuffer(std::shared_ptr<ParticleEmitter> emitter)
{
    auto it = m_emitterBuffers.find(emitter);
    if (it == m_emitterBuffers.end())
    {
        auto buff = std::make_unique<GPUParticleBuffer>(emitter->GetMaxParticles());
        GPUParticleBuffer *buffPtr = buff.get();
        m_emitterBuffers[emitter] = std::move(buff);
        return buffPtr;
    }

    if (it->second->GetMaxParticles() != emitter->GetMaxParticles())
    {
        std::cout << "[ParticleSystem]: Resize GPU Buffer: " << it->second->GetMaxParticles() << " -> " << emitter->GetMaxParticles() << std::endl;
        it->second = std::make_unique<GPUParticleBuffer>(emitter->GetMaxParticles());
        emitter->ResetInsertionIndex();
    }
    return it->second.get();
}

void ParticleSystem::Update(GameWorld &gameWorld, float dt)
{
    auto entities = gameWorld.GetEntitiesWith<ParticleEmitterComponent, TransformComponent>();
    // 实体携带粒子
    for (auto *entity : entities)
    {
        auto &ec = entity->GetComponent<ParticleEmitterComponent>();
        auto &ownerTf = entity->GetComponent<TransformComponent>();
        if (!ec.activate)
            continue;
        for (auto &emitter : ec.emitters)
        {
            // 生成GPU粒子
            GPUParticleBuffer *buffer = GetOrCreateBuffer(emitter);
            emitter->Update(dt, ownerTf, *buffer);
            if (emitter->GetUpdateShader())
            {
                // emitter->PrepareForces(ownerTf); 计算力的在随体系与世界系的变换
                m_TFBManager->Simulate(*(emitter->GetUpdateShader()), *buffer, (int)emitter->GetMaxParticles(), dt);
            }
        }
    }

    // 遗留粒子
    for (auto it = m_orphans.begin(); it != m_orphans.end();)
    {
        GPUParticleBuffer *buffer = GetOrCreateBuffer(it->emitter);

        if (it->emitter->GetUpdateShader())
        {
            // it->emitter->PrepareForces(it->lastTransform);计算力的在随体系与世界系的变换
            m_TFBManager->Simulate(*(it->emitter->GetUpdateShader()), *buffer, (int)it->emitter->GetMaxParticles(), dt);
        }
        if (it->emitter->IsFinished())
        {
            m_emitterBuffers.erase(it->emitter);
            it = m_orphans.erase(it);
        }
        else
            ++it;
    }
}

void ParticleSystem::RegisterOrphan(std::shared_ptr<ParticleEmitter> emitter, const TransformComponent &lastTf)
{
    emitter->SetEmissionRate(0.0f);
    m_orphans.push_back({emitter, lastTf});
}

#if defined(PLATFORM_WEB)
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include "external/glad.h"
#endif

void ParticleSystem::Render(const Texture2D &sceneDepth, float realTime, float gameTime, const Matrix4f &VP, GameWorld &gameWorld, mCamera &camera)
{
    rlDrawRenderBatchActive();
    rlEnableDepthTest();
    rlDisableDepthMask();
    rlDisableBackfaceCulling();

    auto entities = gameWorld.GetEntitiesWith<ParticleEmitterComponent, TransformComponent>();
    for (auto *entity : entities)
    {
        auto &ec = entity->GetComponent<ParticleEmitterComponent>();
        auto &ownerTf = entity->GetComponent<TransformComponent>();
        if (!ec.activate)
            continue;
        for (auto &emitter : ec.emitters)
        {
            GPUParticleBuffer *buffer = GetOrCreateBuffer(emitter);
            if (!buffer)
                continue;
            Matrix4f renderModelMat = emitter->GetRenderMatrix(ownerTf);
            emitter->Render(*buffer, sceneDepth, renderModelMat, camera.Position(),
                            realTime, gameTime, VP, camera);
        }
    }
    // 更新遗留粒子
    for (auto &orphan : m_orphans)
    {
        GPUParticleBuffer *buffer = GetOrCreateBuffer(orphan.emitter);
        if (!buffer)
            continue;

        Matrix4f renderModelMat = orphan.emitter->GetRenderMatrix(orphan.lastTransform);
        orphan.emitter->Render(*buffer, sceneDepth, renderModelMat, camera.Position(),
                               realTime, gameTime, VP, camera);
    }
    glDepthMask(GL_TRUE);
}
