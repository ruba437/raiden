#ifndef MENUBACKGROUND_HPP
#define MENUBACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class MenuBackground : public Util::GameObject {
public:
    MenuBackground() {
        // ⚠️ 請將這裡的檔名替換成你實際準備的主畫面圖片 (例如 title.png 或 menu_bg.png)
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Background/mainScreen.png");
        m_ZIndex = 0; // 放在最底層
        m_Transform.scale = {0.4f, 0.4f};
        m_Transform.translation = {0.0f, 0.0f}; // 放在畫面正中央
    }
};

#endif