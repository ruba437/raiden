#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <algorithm>

class Player : public Util::GameObject {
public:
    enum class WeaponType { DEFAULT, LASER };

private:
    // 儲存三種狀態的圖片指標
    std::shared_ptr<Util::Image> m_ImgStraight;
    std::shared_ptr<Util::Image> m_ImgLeft;
    std::shared_ptr<Util::Image> m_ImgRight;
    int m_WeaponLevel = 1;
    int m_HP = 99;
    WeaponType m_WeaponType = WeaponType::DEFAULT; // 預設武器為散彈
    int m_MissileLevel = 0;
    int m_BombCount = 3;

public:
    Player() {
        // 設定玩家圖片路徑
        m_ImgStraight = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship.png");
        m_ImgLeft = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship_left.png");
        m_ImgRight = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Character/ship_right.png");
        m_Drawable = m_ImgStraight;
        m_ZIndex = 10; // 設定層級，確保飛機在背景上方
    }



    WeaponType GetWeaponType() const { return m_WeaponType; }

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
        float limitX = 300.0f - halfSize.x;
        float limitY = 360.0f - halfSize.y;

        // 4. 使用 std::clamp 限制座標
        m_Transform.translation.x = std::clamp(m_Transform.translation.x, -limitX, limitX);
        m_Transform.translation.y = std::clamp(m_Transform.translation.y, -limitY, limitY);
    }

    // 取得當前位置
    glm::vec2 GetPosition() const { return m_Transform.translation; }

    int GetWeaponLevel() const { return m_WeaponLevel; }

    // 切換武器並升級
    void ChangeWeapon(WeaponType newType) {
        if (m_WeaponType == newType) {
            // 如果吃到同一種武器，就升級
            if (m_WeaponLevel < 5) m_WeaponLevel++;
        } else {
            // 如果吃到不同種武器，切換過去，但火力等級通常會重置為 1 或保持不變
            // 這裡我們先設定為保持等級，玩起來比較爽快
            m_WeaponType = newType;
        }
    }

    int GetMissileLevel() const { return m_MissileLevel; }

    void UpgradeMissile() {
        if (m_MissileLevel < 5) m_MissileLevel++;
    }

    int GetHP() const { return m_HP; }

    // 受到傷害
    void TakeDamage(int damage) {
        m_HP -= damage;
        if (m_HP < 0) m_HP = 0; // 避免血量變成負數
    }

    // 判斷是否死亡
    bool IsDead() const { return m_HP <= 0; }

    void SetPosition(const glm::vec2& pos) {
        m_Transform.translation = pos;
    }

    int GetBombCount() const { return m_BombCount; }

    // 消耗炸彈，回傳是否成功使用
    bool UseBomb() {
        if (m_BombCount > 0) {
            m_BombCount--;
            return true;
        }
        return false;
    }

    // 吃到道具時增加炸彈
    void AddBomb() {
        m_BombCount++;
    }
};

#endif