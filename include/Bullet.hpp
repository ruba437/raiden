#ifndef BULLET_HPP
#define BULLET_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/glm.hpp>
#include <string> // 為了使用 std::string

class Bullet : public Util::GameObject {
private:
    glm::vec2 m_Velocity; // 儲存這顆子彈的飛行速度與方向

public:
    // 修改建構子：新增一個 imagePath 參數，並設定預設值為玩家子彈的圖片
    Bullet(const glm::vec2& position,
           const glm::vec2& velocity = {0.0f, 10.0f},
           const std::string& imagePath = RESOURCE_DIR "/Image/bullet/missile_1.png") // <--- 修改這裡
        : m_Velocity(velocity) {

        // 使用傳入的 imagePath 來載入圖片
        m_Drawable = std::make_shared<Util::Image>(imagePath);
        m_Transform.translation = position;
        m_ZIndex = 5;

        // (進階選用) 如果你之前有加上根據速度旋轉圖片的邏輯，請保留在這裡：
        // if (velocity.x != 0.0f || velocity.y != 10.0f) { // 只有非預設向上的子彈才旋轉
        //     m_Transform.rotation = std::atan2(velocity.y, velocity.x) - 1.5708f;
        // }
    }

    void Update() {
        // 根據專屬的速度向量來移動
        m_Transform.translation += m_Velocity;
    }

    glm::vec2 GetPosition() const {
        return m_Transform.translation;
    }
};

#endif