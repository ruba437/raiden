#ifndef SCOREUI_HPP
#define SCOREUI_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Color.hpp"
#include <string>

class ScoreUI : public Util::GameObject {
public:
    ScoreUI() {
        m_ZIndex = 100; // 設定為最高層級，確保文字不會被背景或飛機蓋住
        m_Transform.translation = {-200.0f, 250.0f}; // 預設放在畫面左上角
        UpdateScore(0); // 初始化顯示 0 分
    }

    void UpdateScore(int score) {
        // 每次分數更新時，重新產生一張文字圖片

        m_Drawable = std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/arial.ttf", 
            30, // 字體大小
            "Score: " + std::to_string(score), 
            Util::Color{255, 255, 255, 255} // 白色字體 (RGBA)
        );
    }
};

class HpUI : public Util::GameObject {
public:
    HpUI() {
        m_ZIndex = 100; // 在最上層
        m_Transform.translation = {-200.0f, -250.0f}; // 放在畫面左下角
        UpdateHP(100);
    }

    void UpdateHP(int hp) {
        m_Drawable = std::make_shared<Util::Text>(
            RESOURCE_DIR "/font/Arial.ttf",
            30,
            "HP: " + std::to_string(hp),
            Util::Color{100, 255, 100, 255}
        );
    }
};

class BombUI : public Util::GameObject {
public:
    BombUI() {
        m_ZIndex = 100;
        m_Transform.translation = {-200.0f, -300.0f};
        UpdateBomb(3);
    }

    void UpdateBomb(int bomb) {
        m_Drawable = std::make_shared<Util::Text>(
            RESOURCE_DIR "/font/Arial.ttf",
            30,
            "Bomb: " + std::to_string(bomb),
            Util::Color{100, 255, 100, 255} // 綠色字體
        );
    }
};

#endif