#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <algorithm>

class Player : public Util::GameObject {
private:
    // 儲存三種狀態的圖片指標
    std::shared_ptr<Util::Image> m_ImgStraight;
    std::shared_ptr<Util::Image> m_ImgLeft;
    std::shared_ptr<Util::Image> m_ImgRight;
    int m_WeaponLevel = 1;
    int m_HP = 3;

public:
    Player() {
        // 設定玩家圖片路徑
        m_ImgStraight = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship.png");
        m_ImgLeft = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship_left.png");
        m_ImgRight = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship_right.png");
        m_Drawable = m_ImgStraight;
        m_ZIndex = 10; // 設定層級，確保飛機在背景上方
    }

    // 根據 X 軸的移動方向來切換圖片
    void SetDirection(float dx) {
        if (dx < 0.0f) {
            m_Drawable = m_ImgLeft;    // 往左移，切換左傾圖
        } else if (dx > 0.0f) {
            m_Drawable = m_ImgRight;   // 往右移，切換右傾圖
        } else {
            m_Drawable = m_ImgStraight;// 沒左右移，恢復直飛
        }
    }

    void Move(const glm::vec2& direction, float speed) {
        m_Transform.translation += direction * speed;

        // 2. 取得圖片大小的一半（避免飛機的一半翅膀飛出去）
        glm::vec2 halfSize = m_Drawable->GetSize() * m_Transform.scale / 2.0f;

        // 3. 設定邊界（根據你的視窗大小調整，這裡假設視窗寬 720, 高 600）
        float limitX = 305.0f - halfSize.x;
        float limitY = 360.0f - halfSize.y;

        // 4. 使用 std::clamp 限制座標
        m_Transform.translation.x = std::clamp(m_Transform.translation.x, -limitX, limitX);
        m_Transform.translation.y = std::clamp(m_Transform.translation.y, -limitY, limitY);
    }

    // 取得當前位置
    glm::vec2 GetPosition() const { return m_Transform.translation; }

    int GetWeaponLevel() const { return m_WeaponLevel; }

    // 升級武器
    void UpgradeWeapon() {
        // 假設最高升級到等級 3
        if (m_WeaponLevel < 3) {
            m_WeaponLevel++;
        }
    }

    int GetHP() const { return m_HP; }

    // 受到傷害
    void TakeDamage(int damage) {
        m_HP -= damage;
        if (m_HP < 0) m_HP = 0; // 避免血量變成負數
    }

    // 判斷是否死亡
    bool IsDead() const { return m_HP <= 0; }
};

#endif