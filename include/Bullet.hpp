#ifndef BULLET_HPP
#define BULLET_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/glm.hpp>

class Bullet : public Util::GameObject {
private:
    glm::vec2 m_Velocity;
public:
    Bullet(const glm::vec2& position, const glm::vec2& velocity = {0.0f, 10.0f})
        : m_Velocity(velocity) {

        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/bullet/missile_1.png");
        m_Transform.translation = position;
        m_ZIndex = 5; 
    }

    void Update() {
        m_Transform.translation += m_Velocity;
    }

    glm::vec2 GetPosition() const {
        return m_Transform.translation;
    }
};

#endif