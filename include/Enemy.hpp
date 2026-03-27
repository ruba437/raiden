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

        // --- 修改：只計算敵機與玩家在 Y 軸上的絕對距離 ---
        float distY = std::abs(m_Transform.translation.y - playerPos.y);

        // 單一突擊型邏輯：
        if (distY < 150.0f) {
            // 當進入玩家上方的 Y 軸警戒帶時：減速並開始攻擊
            m_Transform.translation.y -= 1.5f;

            m_CanShoot = true;
        } else {
            // 還沒到達該高度時：快速進場
            m_Transform.translation.y -= 4.0f;
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