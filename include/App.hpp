#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export
#include "Player.hpp"
#include "Bullet.hpp"
#include "Background.hpp"
#include "Enemy.hpp"
#include "Item.hpp"
#include <memory>
#include <vector>

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ValidTask();

private:
    State m_CurrentState = State::START;
    std::shared_ptr<Player> m_Player;
    std::vector<std::shared_ptr<Bullet>> m_Bullets;
    std::shared_ptr<Background> m_Bg1;
    std::shared_ptr<Background> m_Bg2;

    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    float m_EnemySpawnTimer = 0.0f;

    std::vector<std::shared_ptr<Item>> m_Items;
    std::vector<std::shared_ptr<Bullet>> m_EnemyBullets;
};

#endif
