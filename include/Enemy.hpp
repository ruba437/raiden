#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/glm.hpp>
#include <cstdlib>
#include <string>

class Enemy : public Util::GameObject {
protected:
    // 將變數設為 protected，這樣繼承它的子類別才可以使用這些變數
    int m_HP;
    float m_ShootTimer;
    bool m_CanShoot = false;

public:
    // 建構子：要求傳入初始位置、圖片路徑與血量
    Enemy(const glm::vec2& startPosition, const std::string& imagePath, int hp)
        : m_HP(hp) {

        m_Drawable = std::make_shared<Util::Image>(imagePath);
        m_Transform.translation = startPosition;
        m_ZIndex = 8;
        m_ShootTimer = 30.0f + (std::rand() % 60);
    }

    // 加上虛擬解構子，確保子類別被刪除時記憶體能正確釋放
    virtual ~Enemy() = default;

    // 將 Update 宣告為 virtual，允許子類別覆寫 (Override) 自己的飛行邏輯
    virtual void Update(const glm::vec2& playerPos) {
        // 父類別可以保留共通的邏輯，例如射擊計時器遞減
        if (m_ShootTimer > 0) m_ShootTimer -= 1.0f;
    }

    // 這些共通邏輯通常不需要被子類別修改，所以不一定需要 virtual，但加上也無妨
    virtual bool ReadyToShoot() const {
        return m_ShootTimer <= 0.0f && m_CanShoot;
    }

    virtual void ResetShootTimer() {
        m_ShootTimer = 60.0f + (std::rand() % 40);
    }

    virtual void TakeDamage(int damage) { m_HP -= damage; }
    virtual bool IsDead() const { return m_HP <= 0; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }


    // 預設行為：回傳一顆垂直往下的子彈
    virtual std::vector<glm::vec2> GetBulletVelocities(const glm::vec2& playerPos) const {
        return { glm::vec2(0.0f, -8.0f) };
    }
};

#endif