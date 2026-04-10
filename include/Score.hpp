#ifndef SCOREUI_HPP
#define SCOREUI_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Color.hpp"
#include <string>
#include "Util/Image.hpp"
#include <glm/glm.hpp>

class IconUI : public Util::GameObject {
public:
    IconUI(const std::string& imagePath, glm::vec2 pos, glm::vec2 scale) {
        m_Drawable = std::make_shared<Util::Image>(imagePath);
        // 在子類別內部直接修改 m_Transform，這樣就不會報錯了
        m_Transform.translation = pos;
        m_Transform.scale = scale;
        m_ZIndex = 100;
    }
};

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
            Util::Color{100, 255, 100, 255}
        );
    }
};

class HpUI : public Util::GameObject {
private:
    std::shared_ptr<IconUI> m_HpIcon; // 改用我們新寫的 IconUI
public:
    HpUI() {
        m_ZIndex = 100;
        m_Transform.translation = {-200.0f, -250.0f};

        // 建立飛機圖示 (傳入路徑、位置、縮放比例)
        // 注意：請確認你的資料夾大小寫，如果是 Image 就用大寫 I
        m_HpIcon = std::make_shared<IconUI>(RESOURCE_DIR "/Image/Character/health.png",
                                            glm::vec2{-270.0f, -250.0f},
                                            glm::vec2{0.5f, 0.5f});

        UpdateHP(100);
    }

    void UpdateHP(int hp) {
        m_Drawable = std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/arial.ttf", // 配合你 ScoreUI 的大小寫
            30,
            " X " + std::to_string(hp),
            Util::Color{255, 255, 255, 255}
        );
    }

    void DrawUI() {
        if (m_HpIcon) m_HpIcon->Draw();
        this->Draw();
    }
};

class BombUI : public Util::GameObject {
private:
    std::shared_ptr<IconUI> m_BombIcon; // 現在只需要一個圖示
public:
    BombUI() {
        m_ZIndex = 100;
        // 1. 設定文字的位置 (放在畫面右下角)
        m_Transform.translation = {-200.0f, -300.0f};

        // 2. 建立炸彈圖示，並把它放在文字的左邊 (X座標 150.0f)
        m_BombIcon = std::make_shared<IconUI>(RESOURCE_DIR "/Image/character/bomb.png",
                                             glm::vec2{-270.0f, -300.0f},
                                             glm::vec2{0.6f, 0.6f});

        UpdateBomb(3); // 初始化為 3 顆
    }

    void UpdateBomb(int bombCount) {
        // 更新文字內容，現在只顯示數字
        m_Drawable = std::make_shared<Util::Text>(
            RESOURCE_DIR "/Font/arial.ttf",
            30,
            " X " + std::to_string(bombCount),
            Util::Color{255, 255, 255, 255}
        );
    }

    void DrawUI() {
        if (m_BombIcon) m_BombIcon->Draw(); // 先畫出炸彈圖示
        this->Draw();                       // 再畫出自己身上的數字文字
    }
};

#endif