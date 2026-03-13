#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Player : public Util::GameObject {
public:
    Player() {
        // 設定玩家圖片路徑，記得圖片要放在 Resources/Image/ 內
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship.png");
        m_ZIndex = 10; // 設定層級，確保飛機在背景上方
    }

    void Move(const glm::vec2& direction, float speed) {
        // 更新座標
        m_Transform.translation += direction * speed;
    }

    // 取得當前位置
    glm::vec2 GetPosition() const { return m_Transform.translation; }
};

#endif