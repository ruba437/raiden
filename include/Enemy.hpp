#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Enemy : public Util::GameObject {
public:
    // 建構子接收一個起始座標
    Enemy(const glm::vec2& startPosition) {
        // 請確保 Resources/Image/ 內有 enemy.png
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Enemy/Enemy_plane_1.png");
        m_Transform.translation = startPosition;
        m_ZIndex = 8; // 層級設定在背景之上，玩家之下
    }

    void Update() {
        // 敵機向下移動（速度可以自行調整）
        m_Transform.translation.y -= 3.0f;
    }

    // 取得當前位置（之後做碰撞偵測會用到）
    glm::vec2 GetPosition() const { 
        return m_Transform.translation; 
    }
};

#endif