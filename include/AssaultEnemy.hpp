#ifndef ASSAULT_ENEMY_HPP
#define ASSAULT_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class AssaultEnemy : public Enemy {
private:
    bool m_IsRetreating = false;

    // --- 新增：當前速度變數，預設為最高衝刺速度 ---
    float m_Speed = 8.0f;

public:
    AssaultEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/assault.png", 5) {
        m_CanShoot = false;
    }

    void Update(const glm::vec2& playerPos) override {

        glm::vec2 currentPos = m_Transform.translation;

        if (!m_IsRetreating) {
            // ==========================================
            // 狀態 1：往下衝刺 (並在靠近折返點時減速)
            // ==========================================

            // 假設折返點是 -150.0f，我們設定在距離折返點一段距離前 (例如 Y < 100.0f) 開始煞車
            if (currentPos.y < 50.0f) {
                m_Speed -= 0.2f; // 每一幀減少一點速度 (煞車力度)

                // 設定最低速度，防止減速到 0 卡在半空中下不去
                if (m_Speed < 2.0f) {
                    m_Speed = 2.0f;
                }
            }

            // 使用動態的 m_Speed 來移動
            m_Transform.translation.y -= m_Speed;

            // 計算看向玩家的角度 (弧度)
            float dx = playerPos.x - currentPos.x;
            float dy = playerPos.y - currentPos.y;
            float angleRad = std::atan2(dy, dx);
            m_Transform.rotation = angleRad - 1.570796f;

            // 當到達畫面中下方 (-150.0f) 時，觸發折返
            if (m_Transform.translation.y <= -150.0f) {
                m_IsRetreating = true;

                // --- 關鍵：折返瞬間將速度歸零，準備重新加速 ---
                m_Speed = 0.0f;
            }
        } else {
            // ==========================================
            // 狀態 2：往上折返逃離 (引擎重新點火，逐漸加速)
            // ==========================================

            m_Speed += 0.4f; // 每一幀增加速度 (加速力度)

            // 設定最高逃離速度，避免飛太快破圖或瞬間消失
            if (m_Speed > 12.0f) {
                m_Speed = 12.0f;
            }

            m_Transform.translation.y += m_Speed;
            m_Transform.rotation = 0.0f;
        }

        Enemy::Update(playerPos);
    }

    bool ReadyToShoot() const override { return false; }
};

#endif