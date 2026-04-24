#ifndef SPIRALENEMY_HPP
#define SPIRALENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class SpiralEnemy : public Enemy {
private:
    enum class State { LOOPING, CHARGING };

    State m_State = State::LOOPING;
    glm::vec2 m_LoopCenter;      // 盤旋圓心（隨幀移動）
    float m_Angle = 0.0f;        // 當前盤旋角度（弧度）
    float m_AngleSpeed = 0.1f;   // 正值=逆時針，負值=順時針
    float m_SpinProgress = 0.0f; // 累計旋轉進度（永遠以正值累加）
    glm::vec2 m_ChargeVelocity = {0.0f, 0.0f};  // 鎖定玩家後的衝刺速度向量

public:
    explicit SpiralEnemy(const glm::vec2& startPos)
        : Enemy(startPos, RESOURCE_DIR "/Image/enemy/spiral.png", 3),
          m_LoopCenter(startPos),
          m_ChargeVelocity(0.0f, 0.0f)
    {
        if (startPos.x > 0.0f) {
            // 從右側進場：從圓右側外緣開始，順時針
            m_AngleSpeed = -0.1f;
            m_Angle = 0.0f;
        } else {
            // 從左側進場：從圓左側外緣開始，逆時針
            m_AngleSpeed = 0.1f;
            m_Angle = 3.14159f;
        }
        m_SpinProgress = 0.0f;
        m_CanShoot = false; // 螺旋敵機不射擊，靠衝撞傷害
    }

    void Update(const glm::vec2& playerPos) override {
        glm::vec2 oldPos = m_Transform.translation;

        // 父類別計時器遞減（保留共用邏輯）
        Enemy::Update(playerPos);

        if (m_State == State::LOOPING) {
            // 圓心緩慢向中央 X = 0 靠近
            const float centerSpeed = 2.0f;
            if (m_LoopCenter.x > 0.0f) {
                m_LoopCenter.x -= centerSpeed;
                if (m_LoopCenter.x < 0.0f) m_LoopCenter.x = 0.0f;
            } else if (m_LoopCenter.x < 0.0f) {
                m_LoopCenter.x += centerSpeed;
                if (m_LoopCenter.x > 0.0f) m_LoopCenter.x = 0.0f;
            }

            // 圓心緩慢往下移動
            m_LoopCenter.y -= 0.5f;

            // 實際畫圓角度 + 正向旋轉進度條（避免左側進場提早觸發）
            m_Angle += m_AngleSpeed;
            m_SpinProgress += 0.1f;

            // 利用三角函數計算繞圓位置（半徑 80px）
            const float radius = 50.0f;
            m_Transform.translation.x = m_LoopCenter.x + std::cos(m_Angle) * radius;
            m_Transform.translation.y = m_LoopCenter.y + std::sin(m_Angle) * radius;

            if (m_SpinProgress >= 2.0f * 3.14159f) {
                m_State = State::CHARGING;

                // 鎖定當前玩家位置，計算衝刺方向
                glm::vec2 dir = playerPos - m_Transform.translation;
                float len = glm::length(dir);
                if (len > 0.0f) {
                    dir /= len; // 正規化
                }
                m_ChargeVelocity = dir * 8.0f;
            }
        }
        else if (m_State == State::CHARGING) {
            // 直線衝刺：沿鎖定方向以固定速度前進
            m_Transform.translation += m_ChargeVelocity;
        }

        glm::vec2 velocity = m_Transform.translation - oldPos;
        if (glm::length(velocity) > 0.001f) {
            // atan2 以 +X 方向為 0 弧度；若素材預設朝上可改成 -1.570796f 偏移
            m_Transform.rotation = std::atan2(velocity.y, velocity.x);
        }
    }
};

#endif

