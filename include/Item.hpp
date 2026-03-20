#ifndef ITEM_HPP
#define ITEM_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Item : public Util::GameObject {
public:
    enum class Type { WEAPON_UPGRADE, WEAPON_LASER, SCORE_BONUS };

    Item(const glm::vec2& startPosition, Type type) : m_Type(type) {
        UpdateImage(); // 初始化時設定圖片

        m_Transform.translation = startPosition;
        m_ZIndex = 7;
        m_Transform.scale = {0.5f, 0.5f}; // 保持你之前設定的縮放
    }

    void Update() {
        // --- 1. 如果是分數道具 ---
        if (m_Type == Type::SCORE_BONUS) {
            // 分數道具停在原地，不移動也不變色，所以直接 return 結束更新
            return;
        }

        // 1. 道具緩慢往下飄
        m_Transform.translation.y -= 1.0f;

        // 2. 變換計時器邏輯
        m_ChangeTimer -= 1.0f;
        if (m_ChangeTimer <= 0.0f) {

            // 輪流切換道具種類
            if (m_Type == Type::WEAPON_UPGRADE) {
                m_Type = Type::WEAPON_LASER;
            } else {
                m_Type = Type::WEAPON_UPGRADE;
            }

            // 種類改變後，更新對應的圖片
            UpdateImage();

            // 重置計時器 (假設 60 幀約為 1 秒變換一次，你可以調整這個速度)
            m_ChangeTimer = 180.0f;
        }
    }

    glm::vec2 GetPosition() const { return m_Transform.translation; }
    Type GetType() const { return m_Type; }

private:
    Type m_Type;
    float m_ChangeTimer = 120.0f; // 控制變換頻率的計時器

    // 輔助函式：根據目前的 m_Type 來決定要顯示哪張圖片
    void UpdateImage() {
        if (m_Type == Type::WEAPON_UPGRADE) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/missile_upgrade.png");
        }else if (m_Type == Type::WEAPON_LASER) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/laser_upgrade.png");
        }else {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/gold_medal.png");
        }
    }
};

#endif