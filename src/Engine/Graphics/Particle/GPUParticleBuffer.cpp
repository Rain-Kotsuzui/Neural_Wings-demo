#include "GPUParticleBuffer.h"
#include "rlgl.h"

#if defined(PLATFORM_WEB)
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include "external/glad.h"
#endif

GPUParticleBuffer::GPUParticleBuffer(size_t maxParticles)
{
    m_maxParticles = maxParticles;
    SetupBuffers();
    SetupRenderVBO();
}

GPUParticleBuffer::~GPUParticleBuffer()
{
    glDeleteBuffers(2, m_vbos);
    glDeleteVertexArrays(2, m_vaos);
}

#define BIND_ATTRIB(loc, member, type, count) \
    glEnableVertexAttribArray(loc);           \
    glVertexAttribPointer(loc, count, type, GL_FALSE, sizeof(GPUParticle), (void *)offsetof(GPUParticle, member))

void GPUParticleBuffer::SetupBuffers()
{
    glGenVertexArrays(2, m_vaos);
    glGenBuffers(2, m_vbos);
    size_t totalSize = m_maxParticles * sizeof(GPUParticle);

    for (size_t i = 0; i < 2; i++)
    {
        glBindVertexArray(m_vaos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);

        // 分配显存
        glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_DYNAMIC_DRAW);
        size_t stride = sizeof(GPUParticle);

        // 设置属性指针
        BIND_ATTRIB(0, position, GL_FLOAT, 3);
        BIND_ATTRIB(1, velocity, GL_FLOAT, 3);
        BIND_ATTRIB(2, acceleration, GL_FLOAT, 3);
        BIND_ATTRIB(3, color, GL_FLOAT, 4);
        BIND_ATTRIB(4, size, GL_FLOAT, 2);
        BIND_ATTRIB(5, rotation, GL_FLOAT, 1);
        BIND_ATTRIB(6, life, GL_FLOAT, 2);

        glEnableVertexAttribArray(7);
        glVertexAttribIPointer(7, 1, GL_UNSIGNED_INT, stride, (void *)offsetof(GPUParticle, randomID));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void GPUParticleBuffer::Swap()
{
    m_readIdx ^= 1;
}

void GPUParticleBuffer::UpdateSubData(const std::vector<GPUParticle> &newParticles, size_t offset)
{
    if (newParticles.empty())
        return;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[m_readIdx]);
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(GPUParticle), newParticles.size() * sizeof(GPUParticle), newParticles.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticleBuffer::SetupRenderVBO()

{
    // 格式 Pos(3)+UV(2)
    // XY平面，两个三角形组成
    /*
    0 3
    1 2
    */
    float quadVertices[] =
        {
            //
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 1
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // 2
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // 0
                                            //
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // 2
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // 3
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // 0
        };
    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glGenVertexArrays(2, m_renderVAOS);
    for (int i = 0; i < 2; ++i)
    {
        glBindVertexArray(m_renderVAOS[i]);
        // 绑定顶点属性
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        // loc 0: vertexPosition
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        // loc 1: vertexTexCoord
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

        // 绑定粒子数据
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
        size_t stride = sizeof(GPUParticle);

        BIND_ATTRIB(2, position, GL_FLOAT, 3);
        glVertexAttribDivisor(2, 1);
        BIND_ATTRIB(3, velocity, GL_FLOAT, 3);
        glVertexAttribDivisor(3, 1);
        BIND_ATTRIB(4, acceleration, GL_FLOAT, 3);
        glVertexAttribDivisor(4, 1);
        BIND_ATTRIB(5, color, GL_FLOAT, 4);
        glVertexAttribDivisor(5, 1);
        BIND_ATTRIB(6, size, GL_FLOAT, 2);
        glVertexAttribDivisor(6, 1);
        BIND_ATTRIB(7, rotation, GL_FLOAT, 1);
        glVertexAttribDivisor(7, 1);
        BIND_ATTRIB(8, life, GL_FLOAT, 2);
        glVertexAttribDivisor(8, 1);

        glEnableVertexAttribArray(9);
        glVertexAttribIPointer(9, 1, GL_UNSIGNED_INT, stride, (void *)offsetof(GPUParticle, randomID));
        glVertexAttribDivisor(9, 1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
void GPUParticleBuffer::BindForRender()
{
    glBindVertexArray(m_renderVAOS[m_readIdx]);
}
