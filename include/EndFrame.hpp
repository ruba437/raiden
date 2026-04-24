#ifndef ENDFRAME_HPP
#define ENDFRAME_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/Color.hpp"
#include <memory>
#include <vector>
#include <string>

// 1. 建立一個專屬的文字物件類別 (繼承自 GameObject，完美符合引擎規範)
class TextObject : public Util::GameObject {
public:
    TextObject(const std::string& text, const glm::vec2& pos) {

        m_Drawable = std::make_shared<Util::Text>(
            RESOURCE_DIR "/font/Arial.ttf",
            30,
            text,
            Util::Color(255, 255, 255, 255)
        );
        m_Transform.translation = pos;
        m_ZIndex = 100; // 確保文字在最頂層
    }
};

// 2. 結算裱框主類別
class EndFrame : public Util::GameObject {
public:
    EndFrame() {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/background/EndBackground.png");
        m_ZIndex = 90;
        m_Transform.translation = {0.0f, 0.0f};
        m_Transform.scale = {1.2f, 1.2f};

        m_Texts.push_back(std::make_shared<TextObject>("TEXT SYSTEM WORKING", glm::vec2{0.0f, 0.0f}));
    }

    void SetStats(int bombs, int gold, int silver, int totalScore) {
        m_Texts.clear();
        // 將每一行文字都變成一個 TextObject 加入陣列
        m_Texts.push_back(std::make_shared<TextObject>(std::to_string(bombs), glm::vec2{60.0f, 130.0f}));
        m_Texts.push_back(std::make_shared<TextObject>(std::to_string(gold), glm::vec2{60.0f, 60.0f}));
        m_Texts.push_back(std::make_shared<TextObject>(std::to_string(silver), glm::vec2{60.0f, -10.0f}));
        m_Texts.push_back(std::make_shared<TextObject>("Score:" + std::to_string(totalScore), glm::vec2{0.0f, -70.0f}));
    }


    void DrawUI() {
        Util::GameObject::Draw();

        // 再呼叫所有文字物件的 Draw 畫出文字
        for (auto& textObj : m_Texts) {
            textObj->Draw();
        }
    }

private:
    std::vector<std::shared_ptr<TextObject>> m_Texts;
};

#endif