#ifndef SPREAD_ENEMY_HPP
#define SPREAD_ENEMY_HPP

#include "Enemy.hpp"

class SpreadEnemy : public Enemy {
private:
    bool m_IsStopped = false;

public:
    // 這種類型的敵人通常比較硬，我們給它 15 滴血
    SpreadEnemy(const glm::vec2& startPosition) 
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/spread.png", 15) {
        
        m_CanShoot = false; // 還沒就位前不開火
    }

    void Update(const glm::vec2& playerPos) override {
        
        if (!m_IsStopped) {
            // 狀態 1：緩慢進場
            m_Transform.translation.y -= 3.0f;
            
            // 假設 200.0f 算是畫面上方，到達後停下
            if (m_Transform.translation.y <= 200.0f) {
                m_IsStopped = true;
                m_CanShoot = true; // 停好後允許開火
            }
        } else {
            // 狀態 2：停留在原地
            // 這裡不需要改變位置，它會一直停著當固定砲台
        }

        Enemy::Update(playerPos);
    }

    // --- 覆寫子彈方向：回傳三發散開的子彈 ---
    std::vector<glm::vec2> GetBulletVelocities() const override {
        return {
            glm::vec2(0.0f, -8.0f),   // 正中間往下
            glm::vec2(-3.5f, -8.0f),  // 左斜角
            glm::vec2(3.5f, -8.0f)    // 右斜角
        };
    }
    
    // 稍微降低它的攻擊頻率，給玩家閃躲的空間
    void ResetShootTimer() override { 
        m_ShootTimer = 80.0f + (std::rand() % 40); 
    }
};

#endif