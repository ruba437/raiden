#ifndef BOSS_ENEMY_HPP
#define BOSS_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class BossEnemy : public Enemy {
private:
    // 定義 Boss 的三種行為狀態
    enum class State { 
        MOVE_DOWN,    // 往下移動到中間
        PHASE1_SHOOT, // 停在中間，發射兩排狙擊彈
        MOVE_UP       // 往上退回上方，並發射四向彈幕
    };
    
    State m_State = State::MOVE_DOWN;
    float m_StateTimer = 0.0f; // 用來紀錄在中間停留的時間

public:
    // Boss 擁有超高血量，例如 150 滴血！
    BossEnemy(const glm::vec2& startPosition) 
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/boss_1.png", 150) {
        
        m_CanShoot = false; // 進場時先不開火
        // 💡 如果你有更大張的 Boss 圖片，可以把路徑換成 "/Image/boss.png"
    }

    void Update(const glm::vec2& playerPos) override {
        // Boss 作為大型機體，我們讓它永遠面朝正下方 (180度 = PI)
        m_Transform.rotation = 3.14159265f; 
        
        glm::vec2 currentPos = GetPosition();

        if (m_State == State::MOVE_DOWN) {
            // ==========================================
            // 狀態 1：往下移動
            // ==========================================
            m_Transform.translation.y -= 2.0f; // Boss 移動通常比較沉穩緩慢
            m_CanShoot = false;

            // 當移動到畫面中間偏上 (例如 Y = 150) 時，切換狀態
            if (m_Transform.translation.y <= 150.0f) {
                m_State = State::PHASE1_SHOOT;
                m_StateTimer = 120.0f; // 預計停在中間 3 秒鐘 (60 * 3)
                m_CanShoot = true;
                m_ShootTimer = 0.0f;   // 立刻開火
            }
        } 
        else if (m_State == State::PHASE1_SHOOT) {
            // ==========================================
            // 狀態 2：停在中間發射兩排子彈
            // ==========================================
            // 位置不變，計時器遞減
            m_StateTimer -= 1.0f;
            m_Transform.translation.y -= 1.0f;
            
            if (m_StateTimer <= 0.0f) {
                m_State = State::MOVE_UP;
                m_CanShoot = true;
                m_ShootTimer = 0.0f; // 切換狀態時也立刻開火
            }
        } 
        else if (m_State == State::MOVE_UP) {
            // ==========================================
            // 狀態 3：往上移動並發射四向彈幕
            // ==========================================
            m_Transform.translation.y += 1.5f;

            // 退回畫面上方邊緣 (例如 Y = 400) 時，重新開始新一輪的迴圈
            if (m_Transform.translation.y >= 400.0f) {
                m_State = State::MOVE_DOWN;
            }
        }

        Enemy::Update(playerPos);
    }

    bool ReadyToShoot() const override {
        // 只要不是在 MOVE_DOWN 進場階段，計時器歸零就可以開火
        return (m_State != State::MOVE_DOWN && m_ShootTimer <= 0.0f);
    }

    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        std::vector<EnemyBulletData> data;

        if (m_State == State::PHASE1_SHOOT) {
            // ----------------------------------------
            // 攻擊 1：面向玩家的兩排長串子彈
            // ----------------------------------------
            float dx = playerPos.x - GetPosition().x;
            float dy = playerPos.y - GetPosition().y;
            float angle = std::atan2(dy, dx);
            
            glm::vec2 baseVel(std::cos(angle) * 7.0f, std::sin(angle) * 7.0f);
            glm::vec2 sideDir(-std::sin(angle), std::cos(angle));
            
            // Boss 體型通常較大，我們把兩排子彈的間距拉寬到 35.0f
            float sideOffset = 35.0f; 

            // 一次射出 4 發形成較長的彈鍊
            for (int i = 0; i < 4; ++i) {
                glm::vec2 currentVel = baseVel * (1.0f - i * 0.05f);
                data.push_back({ sideDir * (-sideOffset), currentVel });
                data.push_back({ sideDir * (sideOffset),  currentVel });
            }
        } 
        else if (m_State == State::MOVE_UP) {
            // ----------------------------------------
            // 攻擊 2：四向彈幕 (X型對角線發射)
            // ----------------------------------------
            float bulletSpeed = 5.0f;
            
            // 分別朝向：左下、右下、左上、右上
            data.push_back({ glm::vec2(0, 0), glm::vec2(-bulletSpeed, -bulletSpeed) });
            data.push_back({ glm::vec2(0, 0), glm::vec2( bulletSpeed, -bulletSpeed) });
            data.push_back({ glm::vec2(0, 0), glm::vec2(-bulletSpeed,  bulletSpeed) });
            data.push_back({ glm::vec2(0, 0), glm::vec2( bulletSpeed,  bulletSpeed) });
        }
        
        return data;
    }
    
    void ResetShootTimer() override { 
        // 根據不同狀態給予不同的攻擊節奏
        if (m_State == State::PHASE1_SHOOT) {
            m_ShootTimer = 60.0f; // 中間狙擊：大約 1 秒一波
        } else if (m_State == State::MOVE_UP) {
            m_ShootTimer = 15.0f; // 向上移動：瘋狂連射 (每 0.25 秒發射一次四向彈)
        }
    }
};

#endif