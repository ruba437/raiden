#ifndef MENUBACKGROUND_HPP
#define MENUBACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class MenuBackground : public Util::GameObject {
public:
    MenuBackground() {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Background/Menu.png");
        m_ZIndex = 0; // 放在最底層
        m_Transform.scale = {0.4f, 0.4f};
        m_Transform.translation = {0.0f, 0.0f}; // 放在畫面正中央
    }
};

#endif