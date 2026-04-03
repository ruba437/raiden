#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Background : public Util::GameObject {
public:
    Background() {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Background/background_4.png");
        m_ZIndex = 0; // 確保背景在最底層
        m_Transform.scale = {0.4f, 0.3f};
    }

    float GetScaledHeight() const {
        return m_Drawable->GetSize().y * m_Transform.scale.y;
    }

    void SetPosition(const glm::vec2& position) {
        m_Transform.translation = position;
    }

    void Update(float speed) {
        // 背景向下移動
        m_Transform.translation.y -= 1;

        // 如果背景完全移出畫面底部（假設畫面高度為 600，中心為 0，底部是 -300）
        // 這裡的數值需根據你的背景圖片高度調整
        float h = GetScaledHeight();
        if (m_Transform.translation.y <= -h) {
            // 移到另一張圖的上方。注意：這裡用 + 2*h 確保精準銜接
            m_Transform.translation.y += h * 2.0f;
        }
    }
};

#endif