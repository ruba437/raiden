#ifndef PHANTOM_ENEMY_HPP
#define PHANTOM_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class PhantomEnemy : public Enemy {
private:
    enum class Phase { TOP_TO_BOTTOM, BOTTOM_TO_TOP };
    Phase m_Phase = Phase::TOP_TO_BOTTOM;

    // --- 修改：區分兩個階段的開火紀錄 ---
    bool m_HasFiredPhase1 = false;
    bool m_HasFiredPhase2 = false;

public:
    PhantomEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/phantom.png", 12) {}

    void Update(const glm::vec2& playerPos) override {
        if (m_Phase == Phase::TOP_TO_BOTTOM) {
            m_Transform.translation.y -= 3.0f;
            m_Transform.rotation = 3.14159265f; // 面向下方

            if (m_Transform.translation.y < -400.0f) {
                m_Phase = Phase::BOTTOM_TO_TOP;
                m_Transform.translation.y = -450.0f;
                m_Transform.rotation = 0.0f; // 瞬間轉向朝上
            }
        }
        else {
            // ==========================================
            // 第二相位：向上衝刺
            // ==========================================
            m_Transform.translation.y += 10.0f;
            m_Transform.rotation = 0.0f; // 永遠面向上面
        }

        Enemy::Update(playerPos);
    }

    // --- 修改：判斷兩個階段的開火時機 ---
    bool ReadyToShoot() const override {
        // 階段 1：下降到 Y < 50 時發射狙擊彈
        if (m_Phase == Phase::TOP_TO_BOTTOM && !m_HasFiredPhase1 && m_Transform.translation.y < 50.0f) {
            return true;
        }
        // 階段 2：上升到 Y > -50 時發射下向四向彈 (衝刺到一半)
        if (m_Phase == Phase::BOTTOM_TO_TOP && !m_HasFiredPhase2 && m_Transform.translation.y > -50.0f) {
            return true;
        }
        return false;
    }

    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        std::vector<EnemyBulletData> data;

        if (m_Phase == Phase::TOP_TO_BOTTOM) {
            // ==========================================
            // 攻擊 1：面向玩家的兩排三發狙擊彈
            // ==========================================
            float dx = playerPos.x - m_Transform.translation.x;
            float dy = playerPos.y - m_Transform.translation.y;
            float angle = std::atan2(dy, dx);

            glm::vec2 baseVel(std::cos(angle) * 7.0f, std::sin(angle) * 7.0f);
            glm::vec2 sideDir(-std::sin(angle), std::cos(angle));
            float sideOffset = 22.0f;

            for (int i = 0; i < 3; ++i) {
                glm::vec2 currentVel = baseVel * (1.0f - i * 0.08f);
                data.push_back({ sideDir * (-sideOffset), currentVel });
                data.push_back({ sideDir * (sideOffset),  currentVel });
            }
        }
        else {
            // ==========================================
            // 攻擊 2：朝向下方的四向擴散彈
            // ==========================================
            // 我們直接定義四個往下的斜向速度
            float bulletSpeed = 6.0f;
            data.push_back({ glm::vec2(0, 0), glm::vec2(-12.0f, -bulletSpeed) }); // 左斜下
            data.push_back({ glm::vec2(0, 0), glm::vec2(-3.0f, -bulletSpeed) }); // 左微斜下
            data.push_back({ glm::vec2(0, 0), glm::vec2( 3.0f, -bulletSpeed) }); // 右微斜下
            data.push_back({ glm::vec2(0, 0), glm::vec2( 12.0f, -bulletSpeed) }); // 右斜下
        }

        return data;
    }

    // --- 修改：根據當前相位重置紀錄 ---
    void ResetShootTimer() override {
        if (m_Phase == Phase::TOP_TO_BOTTOM) {
            m_HasFiredPhase1 = true;
        } else {
            m_HasFiredPhase2 = true;
        }
        // 設定一個很大的時間，防止在同一個相位重複觸發
        m_ShootTimer = 99999.0f;
    }
};

#endif