#ifndef ENDBACKGROUND_HPP
#define ENDBACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class EndBackground : public Util::GameObject {
public:
    EndBackground() {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Background/EndBackground.png");
        m_ZIndex = 20; // 設高一點，蓋在所有東西上面
        m_Transform.translation = {0.0f, 0.0f}; 
    }
};

#endif