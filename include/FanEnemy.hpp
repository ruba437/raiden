#ifndef FAN_ENEMY_HPP
#define FAN_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>
#include <vector>

class FanEnemy : public Enemy {
public:
    // 血量設定為 30，使用 spread.png 作為外觀
    FanEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/fan.png", 10) {
        
        m_CanShoot = true;
        m_ShootTimer = 120.0f; // 出場 1 秒後發動第一次攻擊
    }

    void Update(const glm::vec2& playerPos) override {
        // 進場邏輯：從畫面上方往下移動，直到 Y 座標來到 200.0f 就停住
        if (m_Transform.translation.y > 200.0f) {
            m_Transform.translation.y -= 2.0f;
        }

        // 讓圖片朝下 (180度 = 3.14159 弧度)
        m_Transform.rotation = 3.14159265f;

        // 更新射擊計時器
        Enemy::Update(playerPos);
    }

    // 實作攻擊：同時向下發射半圓形 (扇形) 子彈
    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        std::vector<EnemyBulletData> bullets;
        float speed = 6.0f; // 子彈飛行速度

        int numBullets = 13;      // 一次發射 13 顆子彈 (數量越多越密)
        float startAngle = 0.0f;  // 0 度代表正右方
        float endAngle = -180.0f; // -180 度代表正左方

        // 利用迴圈，瞬間產生 13 顆不同角度的子彈
        for (int i = 0; i < numBullets; ++i) {
            // 計算這顆子彈的角度 (從 0 度一路遞減到 -180 度)
            float angleDeg = startAngle + (endAngle - startAngle) * i / (numBullets - 1);
            
            // 角度轉換成弧度
            float angleRad = angleDeg * 3.14159265f / 180.0f;

            // 計算 X 與 Y 的速度向量
            glm::vec2 velocity = { std::cos(angleRad) * speed, std::sin(angleRad) * speed };
            
            // 把子彈加入陣列中 (假設使用藍色子彈)
            bullets.push_back({ glm::vec2(0, 0), velocity, RESOURCE_DIR "/Image/bullet/yellow.png" });
        }

        // 一次把 13 顆子彈交給 App 去生成！
        return bullets;
    }

    // 重置攻擊計時器
    void ResetShootTimer() override {
        m_ShootTimer = 90.0f; // 每 1.5 秒發射一次半圓形彈幕
    }
};

#endif