#ifndef MAP_OBJECT_ENEMY_HPP
#define MAP_OBJECT_ENEMY_HPP

#include "Enemy.hpp"

class MapObjectEnemy : public Enemy {
public:
    // 💡 這裡的圖片路徑請換成你準備好的地圖物件 (例如石頭、雲朵或建築物)
    // HP 隨便設，因為子彈根本不會碰到它
    MapObjectEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/Background/rock.png", 9999) {
        
        m_CanShoot = false; // 關閉射擊能力
        
        // 🌟 【圖層關鍵】
        // 一般敵人是 8，玩家是 5~7 左右。
        // 我們把它設為 2 或 3，這樣它就會乖乖待在飛機下面，但又在最底層背景(0)之上！
        m_ZIndex = 2; 
    }

    void Update(const glm::vec2& playerPos) override {
        // 隨地圖往下移動
        m_Transform.translation.y -= 0.5f;

        // 不需要旋轉，也不需要發射子彈
        Enemy::Update(playerPos);
    }
    
    // 因為 m_CanShoot 是 false，所以不需要覆寫 GetBulletData
};

#endif