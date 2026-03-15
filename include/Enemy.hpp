#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <cstdlib> // 為了使用 rand()

class Enemy : public Util::GameObject {
private:
    float m_ShootTimer; // 射擊冷卻計時器

public:
    Enemy(const glm::vec2& startPosition) {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Enemy/Enemy_plane_1.png");
        m_Transform.translation = startPosition;
        m_ZIndex = 8;

        // 初始給予一個隨機的冷卻時間 (例如 30~90 幀之間)，避免所有敵機同時開火
        m_ShootTimer = 30.0f + (std::rand() % 60);
    }

    void Update() {
        m_Transform.translation.y -= 2.0f;

        // 計時器遞減
        if (m_ShootTimer > 0) {
            m_ShootTimer -= 1.0f;
        }
    }

    // 判斷是否可以射擊
    bool ReadyToShoot() const {
        return m_ShootTimer <= 0.0f;
    }

    // 發射後重置計時器 (例如每 90~150 幀發射一次)
    void ResetShootTimer() {
        m_ShootTimer = 90.0f + (std::rand() % 60);
    }

    glm::vec2 GetPosition() const { return m_Transform.translation; }
};

#endif