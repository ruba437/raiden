#ifndef ITEM_HPP
#define ITEM_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Item : public Util::GameObject {
public:
    // 定義道具種類，方便未來擴充（例如：加血、大招、火力升級）
    enum class Type { WEAPON_UPGRADE, SCORE_BONUS };

    Item(const glm::vec2& startPosition, Type type) : m_Type(type) {
        // 根據道具種類載入不同的圖片 (請確保 Resources/Image/ 內有對應圖片)
        if (type == Type::WEAPON_UPGRADE) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Item/missile_upgrade.png");
        } else {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/image/Item/missile_upgrade.png");
        }
        
        m_Transform.translation = startPosition;
        m_ZIndex = 7; // 層級設在敵機與玩家之間

        m_Transform.scale = {0.5f, 0.5f};
    }

    void Update() {
        // 道具緩慢往下飄
        //m_Transform.translation.y -= 1.5f;
    }

    glm::vec2 GetPosition() const { return m_Transform.translation; }
    Type GetType() const { return m_Type; }

private:
    Type m_Type;
};

#endif