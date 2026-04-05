#ifndef DECK_HPP
#define DECK_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class Deck : public Util::GameObject {
public:
    Deck() {
        // 確保你的資料夾名稱大小寫正確 (如果是 Image 記得改大寫)
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Background/deck.png");
        m_ZIndex = 5; // 設定在背景 (0) 之上，玩家 (10) 之下
    }

    void SetPosition(const glm::vec2& pos) {
        m_Transform.translation = pos;
    }


};

#endif