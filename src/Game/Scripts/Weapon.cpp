#include "Weapon.h"
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Core/GameWorld.h"
#include <limits>
void BulletScript::OnFixedUpdate(float fixedDeltaTime)
{
    timer += fixedDeltaTime;
    if (timer >= lifeTime)
        owner->GetOwnerWorld()->GetPool("bullet").Recycle(owner);
}
void BulletScript::OnWake()
{

    timer = 0.0f;
}
void BulletScript::Initialize(const json &data)
{
    lifeTime = data.value("lifeTime", 2.0f);
}
void WeaponScript::Initialize(const json &data)
{
    if (data.contains("velocity"))
        m_bulletVelocity = data["velocity"];
    m_fireRate = data.value("fireRate", 0.15f);
}
#include <random>
void WeaponScript::OnUpdate(float deltaTime)
{
    auto &input = owner->GetOwnerWorld()->GetInputManager();
    if (input.IsActionDown("Fire"))
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        m_fireTimer += deltaTime;
        while (m_fireTimer > m_fireRate)
        {
            m_fireTimer -= m_fireRate;
            auto &tf = owner->GetComponent<TransformComponent>();
            float size = owner->GetComponent<RigidbodyComponent>().localAABB.max.z();
            float width = owner->GetComponent<RigidbodyComponent>().localAABB.max.x();
            Vector3f spawnVel = owner->GetComponent<RigidbodyComponent>().velocity;
            Vector3f spawnPos = tf.GetWorldPosition() + tf.GetForward() * (size + 0.5f + dis(gen) * 0.5f) + tf.GetRight() * width;
            // 修改名字以区分不同类型的子弹和owner
            std::string name = "bullet_" + std::to_string(rand());
            GameObject *bullet = owner->GetOwnerWorld()->GetPool("bullet").Spawn(name, "bullet", spawnPos, tf.GetWorldRotation());
            auto &rb = bullet->GetComponent<RigidbodyComponent>();
            rb.velocity = tf.GetForward() * m_bulletVelocity + spawnVel;

            spawnPos = tf.GetWorldPosition() + tf.GetForward() * (size + 0.5f + dis(gen) * 0.5f) - tf.GetRight() * width;
            name = "bullet_" + std::to_string(rand());
            GameObject *bullet2 = owner->GetOwnerWorld()->GetPool("bullet").Spawn(name, "bullet", spawnPos, tf.GetWorldRotation());
            auto &rb2 = bullet2->GetComponent<RigidbodyComponent>();
            rb2.velocity = tf.GetForward() * m_bulletVelocity + spawnVel;

            auto &audio = owner->GetComponent<AudioComponent>();

            float randomPitch = 0.3f + dis(gen) * 1.5f;
            float randomVol = 0.8f + dis(gen) * 0.2f;
            audio.Play("Fire", randomVol, randomPitch);

            // world->GetParticleSystem().Spawn(...);
        }
    }
    else
        m_fireTimer = m_fireRate - std::numeric_limits<float>::min();
}