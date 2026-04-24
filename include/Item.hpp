#ifndef ITEM_HPP
#define ITEM_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <cstdlib>
#include <cmath>

class Item : public Util::GameObject {
public:
    enum class Type { WEAPON_UPGRADE, WEAPON_LASER, WEAPON_MISSILE, SCORE_BONUS, SCORE_BONUS_SILVER, BOMB };

    Item(const glm::vec2& startPosition, Type type) : m_Type(type) {
        UpdateImage(); // 初始化時設定圖片

        m_Transform.translation = startPosition;
        m_ZIndex = 7;
        m_Transform.scale = {0.5f, 0.5f}; // 保持你之前設定的縮放

        if (m_Type == Type::SCORE_BONUS || m_Type == Type::SCORE_BONUS_SILVER) {
            // 分數道具：不隨機移動，只跟著地圖往下掉 (假設地圖滾動速度為 0.5f)
            m_Velocity = {0.0f, -1.0f};
        } else {
            // 武器道具：產生隨機角度，給予初始的 X 和 Y 速度
            float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
            float speed = 2.0f; // 武器道具的移動速度，可以自由調整
            m_Velocity = { std::cos(angle) * speed, std::sin(angle) * speed };
        }
    }

    void Update() {
        // 1. 更新位置
        m_Transform.translation += m_Velocity;

        // --- 2. 如果是分數道具，只要往下掉就好，不處理反彈也不變換 ---
        if (m_Type == Type::SCORE_BONUS || m_Type == Type::SCORE_BONUS_SILVER) {
            return;
        }

        // --- 3. 武器道具的邊界反彈邏輯 ---
        // 假設你的視窗範圍大約如下，如果道具撞到邊緣就會卡住並反向
        const float minX = -290.0f;
        const float maxX =  290.0f;
        const float minY = -350.0f;
        const float maxY =  350.0f;

        // X 軸撞牆反彈
        if (m_Transform.translation.x <= minX) {
            m_Transform.translation.x = minX; // 修正位置，避免卡在牆外
            m_Velocity.x *= -1.0f;            // 速度反向
        } else if (m_Transform.translation.x >= maxX) {
            m_Transform.translation.x = maxX;
            m_Velocity.x *= -1.0f;
        }

        // Y 軸撞牆反彈
        if (m_Transform.translation.y <= minY) {
            m_Transform.translation.y = minY;
            m_Velocity.y *= -1.0f;
        } else if (m_Transform.translation.y >= maxY) {
            m_Transform.translation.y = maxY;
            m_Velocity.y *= -1.0f;
        }

        if (m_Type == Type::BOMB) {
            return;
        }

        // --- 4. 變換計時器邏輯 (只針對武器道具) ---
        m_ChangeTimer -= 1.0f;
        if (m_ChangeTimer <= 0.0f) {
            // 輪流切換道具種類
            if (m_Type == Type::WEAPON_UPGRADE) {
                m_Type = Type::WEAPON_LASER;     // 散彈 -> 雷射
            } else if (m_Type == Type::WEAPON_LASER) {
                m_Type = Type::WEAPON_MISSILE;   // 雷射 -> 飛彈
            } else {
                m_Type = Type::WEAPON_UPGRADE;   // 飛彈 -> 散彈
            }

            // 種類改變後，更新對應的圖片
            UpdateImage();

            // 重置計時器
            m_ChangeTimer = 180.0f;
        }
    }

    glm::vec2 GetPosition() const { return m_Transform.translation; }
    Type GetType() const { return m_Type; }

private:
    Type m_Type;
    float m_ChangeTimer = 120.0f; // 控制變換頻率的計時器
    glm::vec2 m_Velocity;

    // 輔助函式：根據目前的 m_Type 來決定要顯示哪張圖片
    void UpdateImage() {
        if (m_Type == Type::WEAPON_UPGRADE) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/default_upgrade.png");
        }else if (m_Type == Type::WEAPON_LASER) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/laser_upgrade.png");
        }else if (m_Type == Type::WEAPON_MISSILE) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/missile_upgrade.png");
        }else if (m_Type == Type::SCORE_BONUS){
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/gold_medal.png");
        }else if (m_Type == Type::SCORE_BONUS_SILVER) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/silver_medal.png");
        }else if (m_Type == Type::BOMB) {
            m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/item/bomb.png");
        }
    }
};

#endif