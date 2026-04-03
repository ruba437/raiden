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
            if (m_Transform.translation.y <= 250.0f) {
                m_State = State::PHASE1_SHOOT;
                m_StateTimer = 240.0f; // 預計停在中間 3 秒鐘 (60 * 3)
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
            float dx = playerPos.x - GetPosition().x;
            float dy = playerPos.y - GetPosition().y;
            float aimAngle = std::atan2(dy, dx);

            // --- 統一的基礎速度，讓所有子彈感覺是「一體」的 ---
            float baseSpeed = 4.0f;

            // ==========================================
            // 攻擊 1-A：面向玩家的兩排狙擊彈
            // ==========================================
            glm::vec2 baseVel(std::cos(aimAngle) * baseSpeed, std::sin(aimAngle) * baseSpeed);
            glm::vec2 sideDir(-std::sin(aimAngle), std::cos(aimAngle));
            float sideOffset = 35.0f;

            for (int i = 0; i < 4; ++i) {
                glm::vec2 currentVel = baseVel * (1.0f - i * 0.05f);
                data.push_back({ sideDir * (-sideOffset), currentVel, RESOURCE_DIR "/Image/bullet/green.png" });
                data.push_back({ sideDir * (sideOffset),  currentVel, RESOURCE_DIR "/Image/bullet/green.png" });
            }

            // ==========================================
            // 攻擊 1-B：朝向玩家集中的密集 U 字扇形
            // ==========================================
            float halfSpread = 0.7f;
            int fanCount = 11;
            float angleStep = (halfSpread * 2.0f) / (fanCount - 1);

            int centerIndex = fanCount / 2;

            for (int i = 0; i < fanCount; ++i) {
                float currentAngle = (aimAngle - halfSpread) + (i * angleStep);

                // 1. 計算距離中心的比例 (0.0 代表在正中央，1.0 代表在最外側)
                float ratio = std::abs(i - centerIndex) / (float)centerIndex;

                // 2. 決定速度倍率 (改成用減的，讓外側減速)
                // 中央的子彈速度倍率為 1.0 (維持 baseSpeed 7.0f，與兩排狙擊彈完美同步)
                // 最外側的子彈速度倍率為 1.0 - 0.4 = 0.6 (速度降到 4.2f)
                float speedMultiplier = 1.0f - (ratio * ratio * 0.5f);
                float currentSpeed = (baseSpeed + 1) * speedMultiplier;

                data.push_back({ glm::vec2(0, 0), glm::vec2(std::cos(currentAngle) * currentSpeed, std::sin(currentAngle) * currentSpeed), RESOURCE_DIR "/Image/bullet/yellow.png" });
            }
        }
        else if (m_State == State::MOVE_UP) {
            // ==========================================
            // 攻擊 2：偏向下方的四向彈幕 (撤退掩護火力)
            // ==========================================
            float bulletSpeed = 5.0f;

            // 設定正下方為基準角度 (-90度，即 -1.570796f 弧度)
            // 因為畫面 Y 軸往下是負的，所以 sin(-90度) = -1，剛好往下飛
            float downAngle = -1.570796f;

            // 設定四個子彈的角度偏移量 (例如偏 25 度與偏 55 度)
            // 你可以透過修改這些數字來決定子彈張開的幅度
            float angles[4] = {
                downAngle - 0.45f, // 左側靠中
                downAngle + 0.45f, // 右側靠中
                downAngle - 0.95f, // 左側偏外
                downAngle + 0.95f  // 右側偏外
            };

            // 利用迴圈發射這 4 顆子彈
            for (int i = 0; i < 4; ++i) {
                data.push_back({
                    glm::vec2(0, 0),
                    glm::vec2(std::cos(angles[i]) * bulletSpeed, std::sin(angles[i]) * bulletSpeed),
                    RESOURCE_DIR "/Image/bullet/blue.png"
                });
            }
        }

        return data;
    }
    
    void ResetShootTimer() override { 
        // 根據不同狀態給予不同的攻擊節奏
        if (m_State == State::PHASE1_SHOOT) {
            m_ShootTimer = 120.0f; // 中間狙擊：大約 1 秒一波
        } else if (m_State == State::MOVE_UP) {
            m_ShootTimer = 60.0f; // 向上移動：瘋狂連射 (每 0.25 秒發射一次四向彈)
        }
    }
};

#endif