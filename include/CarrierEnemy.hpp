#ifndef CARRIER_ENEMY_HPP
#define CARRIER_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>
#include <vector>

class CarrierEnemy : public Enemy {
private:
    int m_AttackMode = 0; // 0 = 模式一 (上方瞄準), 1 = 模式二 (下方斜角)
    int m_BurstCount = 2; // 控制連發次數 (預設模式一是3發，所以剩2發)

public:
    CarrierEnemy(const glm::vec2& startPosition)
        // 這裡暫時借用 fortress.png (要塞)，你可以換成專屬的運輸機圖片
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/carrier.png", 15) {
        
        m_CanShoot = true;
        m_ShootTimer = 60.0f; // 出場 1 秒後開火
    }

    void Update(const glm::vec2& playerPos) override {
        // 隨著地圖往下移動
        m_Transform.translation.y -= 1.0f;

        // 保持機頭朝下
        m_Transform.rotation = 3.14159265f;

        Enemy::Update(playerPos);
    }

    // 實作攻擊：根據當前的 AttackMode 決定砲口位置與子彈軌跡
    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        std::vector<EnemyBulletData> bullets;
        float speed = 6.0f;

        if (m_AttackMode == 0) {
            // ==========================================
            // 【模式一】：從怪的「上方」朝玩家發射
            // ==========================================
            glm::vec2 topOffset = {0.0f, 60.0f}; // 上方砲口的 Y 軸偏移量
            
            // 💡 專業細節：計算角度時，要從「上方砲口」的真實座標去算，而不是敵機中心！
            glm::vec2 actualFirePos = m_Transform.translation + topOffset;
            float angle = std::atan2(playerPos.y - actualFirePos.y, playerPos.x - actualFirePos.x);
            
            glm::vec2 velocity = { std::cos(angle) * speed, std::sin(angle) * speed };
            
            // 發射紅色子彈
            bullets.push_back({ topOffset, velocity, RESOURCE_DIR "/Image/bullet/blue.png" });
        } 
        else if (m_AttackMode == 1) {
            // ==========================================
            // 【模式二】：從怪的「下方」兩個斜角發射
            // ==========================================
            glm::vec2 bottomOffset = {0.0f, -60.0f}; // 下方砲口的 Y 軸偏移量

            // 左下 (-110度) 與 右下 (-70度)
            float leftAngle = -110.0f * 3.14159265f / 180.0f;
            float rightAngle = -70.0f * 3.14159265f / 180.0f;

            // 發射兩顆藍色子彈
            bullets.push_back({ bottomOffset, glm::vec2(std::cos(leftAngle) * speed, std::sin(leftAngle) * speed), RESOURCE_DIR "/Image/bullet/yellow.png" });
            bullets.push_back({ bottomOffset, glm::vec2(std::cos(rightAngle) * speed, std::sin(rightAngle) * speed), RESOURCE_DIR "/Image/bullet/yellow.png" });
        }

        return bullets;
    }

    // 利用連發計時器來完美實作「兩種模式交替」
    void ResetShootTimer() override {
        if (m_BurstCount > 0) {
            // 【還在連發中】
            m_ShootTimer = 10.0f; // 10 幀後馬上射下一發 (連射)
            m_BurstCount--;
        } else {
            // 【連發結束，切換模式】
            m_AttackMode = (m_AttackMode == 0) ? 1 : 0;
            
            // 進入較長的冷卻時間 (休息 1.5 秒 = 90 幀)
            m_ShootTimer = 90.0f; 

            // 重新裝填下一個模式的子彈數 (總發數減 1，因為第一發不需等待)
            if (m_AttackMode == 0) {
                m_BurstCount = 2; // 上方瞄準：總共 3 發
            } else {
                m_BurstCount = 4; // 下方斜角：總共 5 發 (你可以自由修改)
            }
        }
    }
};

#endif