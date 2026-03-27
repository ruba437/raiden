#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/glm.hpp>
#include <cstdlib>

class Enemy : public Util::GameObject {
private:
    int m_HP;
    float m_ShootTimer;
    bool m_CanShoot = false; // 控制是否進入攻擊狀態

public:
    // 建構子：移除 Type 參數，直接設定血量 (預設給 5)
    Enemy(const glm::vec2& startPosition, int hp = 5)
        : m_HP(hp) {

        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/enemy/attacker.png");
        m_Transform.translation = startPosition;
        m_ZIndex = 8;

        m_ShootTimer = 30.0f + (std::rand() % 60);
    }

    void Update(const glm::vec2& playerPos) {
        // 計算這台敵機與玩家的直線距離
        float distToPlayer = glm::distance(m_Transform.translation, playerPos);

        // 單一突擊型邏輯：
        if (distToPlayer < 350.0f) {
            // 靠近玩家時：減速 (從 -5.0f 降到 -1.5f)，持續往下飛，並開始攻擊
            m_Transform.translation.y -= 1.5f;
            m_CanShoot = true;
        } else {
            // 還沒靠近時：快速進場 (速度 -5.0f)，且不開火
            m_Transform.translation.y -= 5.0f;
            m_CanShoot = false;
        }

        // 射擊計時器遞減
        if (m_ShootTimer > 0) m_ShootTimer -= 1.0f;
    }

    bool ReadyToShoot() const {
        return m_ShootTimer <= 0.0f && m_CanShoot;
    }

    void ResetShootTimer() {
        m_ShootTimer = 60.0f + (std::rand() % 40);
    }

    void TakeDamage(int damage) { m_HP -= damage; }
    bool IsDead() const { return m_HP <= 0; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
};

#endif