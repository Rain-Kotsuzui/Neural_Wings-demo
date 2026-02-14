#include "Weapon.h"
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Core/GameWorld.h"
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

void WeaponScript::OnUpdate(float deltaTime)
{
    auto &input = owner->GetOwnerWorld()->GetInputManager();
    if (input.IsActionDown("Fire"))
    {
        m_fireTimer += deltaTime;
        while (m_fireTimer >= 0.05f)
        {
            m_fireTimer -= 0.05f;
            auto &tf = owner->GetComponent<TransformComponent>();
            Vector3f spawnPos = tf.GetWorldPosition() + tf.GetForward() * (static_cast<float>(rand()) * 1 / RAND_MAX + 1);
            // 修改名字以区分不同类型的子弹和owner
            std::string name = "bullet_" + std::to_string(rand());
            GameObject *bullet = owner->GetOwnerWorld()->GetPool("bullet").Spawn(name, spawnPos, tf.GetWorldRotation());

            auto &rb = bullet->GetComponent<RigidbodyComponent>();
            rb.velocity = tf.GetForward() * 10.0f;

            // TODO:连续声音
            auto &audio = owner->GetComponent<AudioComponent>();
            audio.Play("Fire");

            // world->GetParticleSystem().Spawn(...);
        }
    }
    else
        m_fireTimer = 0.049f;
}