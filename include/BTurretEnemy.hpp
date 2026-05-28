#ifndef BTURRETENEMY_HPP
#define BTURRETENEMY_HPP

#include "Enemy.hpp"

class BTurretEnemy : public Enemy {
private:
    int m_BurstCount = 4; // 記錄連發剩餘次數 (初始設為 4，代表第一波就會連射 5 發)

public:
    // 砲塔設定血量為 20
    BTurretEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/Bturret.png", 7) {

        m_CanShoot = true;
        m_ShootTimer = 60.0f; // 出生後 1 秒開始第一波攻擊
    }

    void Update(const glm::vec2& playerPos) override {
        m_Transform.translation.y -= 1.0f;

        // 砲台不需要瞄準，固定朝下 (180度 = PI)
        m_Transform.rotation = 3.14159265f;

        // 呼叫父類別以處理射擊計時器 (m_ShootTimer) 的遞減
        Enemy::Update(playerPos);
    }

    // 實作攻擊：一次發射向下方的兩個斜角子彈
    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        float speed = 5.0f; // 子彈速度

        // 畫面中，正下方是 -90度 (-1.5708 弧度)
        // 左邊子彈設為 -100 度
        float leftAngle = -100.0f * 3.14159265f / 180.0f;
        // 右邊子彈設為 -80 度
        float rightAngle = -80.0f * 3.14159265f / 180.0f;

        // 回傳這兩顆子彈的資料，App.cpp 會自動幫我們把它們生出來！
        return {
                { glm::vec2(0, 0), glm::vec2(std::cos(leftAngle) * speed, std::sin(leftAngle) * speed), RESOURCE_DIR "/Image/bullet/yellow.png" },
                { glm::vec2(0, 0), glm::vec2(std::cos(rightAngle) * speed, std::sin(rightAngle) * speed), RESOURCE_DIR "/Image/bullet/yellow.png" }
        };
    }

    // 利用覆寫 ResetShootTimer 來實作「連發系統 (Burst)」
    void ResetShootTimer() override {
        if (m_BurstCount > 0) {
            // 還在連發狀態中：給予極短的冷卻時間 (例如 8 幀發射一波)
            m_ShootTimer = 8.0f;
            m_BurstCount--;
        } else {
            // 連發結束：進入長冷卻時間 (例如 120 幀 = 2秒)
            m_ShootTimer = 120.0f;
            // 重新填裝下一波的連發次數 (4 代表後續還有 4 發，加起來共 5 發)
            m_BurstCount = 4;
        }
    }
};

#endif