#ifndef BULLET_HPP
#define BULLET_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Bullet : public Util::GameObject {
public:
    Bullet(const glm::vec2& position) {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Bullet/missile_1.png");
        m_Transform.translation = position;
        m_ZIndex = 5; 
    }

    void Update() {
        // 子彈向上飛
        m_Transform.translation.y += 10.0f;
    }

    glm::vec2 GetPosition() const {
        return m_Transform.translation;
    }
};

#endif