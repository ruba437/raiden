#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <cstdlib>

void App::Start() {
    LOG_TRACE("Start");
    m_Player = std::make_shared<Player>();
    m_CurrentState = State::UPDATE;


    m_Bg1 = std::make_shared<Background>();
    float h = m_Bg1->GetScaledHeight(); // 取得動態計算的高度

    m_Bg1->SetPosition({0, 0});

    m_Bg2 = std::make_shared<Background>();
    m_Bg2->SetPosition({0, h}); // 精準放在第一張上方，不要多 1 像素也不要少

    m_Player = std::make_shared<Player>();

    // 直接在畫面中央偏上方生成一個測試用的武器升級道具
    // 假設 x = 0.0f, y = 200.0f
    auto testItem = std::make_shared<Item>(glm::vec2(0.0f, 200.0f), Item::Type::WEAPON_UPGRADE);
    m_Items.push_back(testItem);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    float bgSpeed = 2.0f;

    // 更新並繪製背景
    m_Bg1->Update(bgSpeed);
    m_Bg2->Update(bgSpeed);
    m_Bg1->Draw();
    m_Bg2->Draw();

    float speed = 5.0f;
    glm::vec2 dir = {0.0f, 0.0f};
    float dx = 0.0f; // 用來紀錄 X 軸的純方向狀態

    // 偵測按鍵
    if (Util::Input::IsKeyPressed(Util::Keycode::W)) dir.y += 1.0f;
    if (Util::Input::IsKeyPressed(Util::Keycode::S)) dir.y -= 1.0f;

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        dir.x -= 1.0f;
        dx = -1.0f;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        dir.x += 1.0f;
        dx = 1.0f;
    }

    // 1. 更新動畫狀態 (傳入 dx)
    m_Player->SetDirection(dx);

    // 2. 更新位置
    if (dir != glm::vec2(0.0f, 0.0f)) {
        m_Player->Move(glm::normalize(dir), speed);
    }

    // --- 2. 偵測攻擊輸入 ---
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) { // 建議配合冷卻時間使用

        glm::vec2 playerPos = m_Player->GetPosition();
        int level = m_Player->GetWeaponLevel();

        if (level == 1) {
            // 等級 1：單發 (正中央)
            m_Bullets.push_back(std::make_shared<Bullet>(playerPos));

        } else if (level == 2) {
            // 等級 2：雙排 (左右各偏移 15 像素)
            glm::vec2 leftPos = {playerPos.x - 15.0f, playerPos.y};
            glm::vec2 rightPos = {playerPos.x + 15.0f, playerPos.y};

            m_Bullets.push_back(std::make_shared<Bullet>(leftPos));
            m_Bullets.push_back(std::make_shared<Bullet>(rightPos));

        } else if (level >= 3) {
            // 等級 3：三排 (正中央 + 左右各偏移 25 像素，且稍微往後一點)
            glm::vec2 leftPos = {playerPos.x - 25.0f, playerPos.y - 10.0f};
            glm::vec2 rightPos = {playerPos.x + 25.0f, playerPos.y - 10.0f};

            m_Bullets.push_back(std::make_shared<Bullet>(playerPos));
            m_Bullets.push_back(std::make_shared<Bullet>(leftPos));
            m_Bullets.push_back(std::make_shared<Bullet>(rightPos));
        }
    }

    // --- 3. 更新與繪製子彈 ---
    for (auto it = m_Bullets.begin(); it != m_Bullets.end(); ) {
        auto& bullet = *it;
        bullet->Update();
        bullet->Draw();

        // 效能優化：如果子彈飛出畫面（假設高度 500），就從 vector 移除
        if (bullet->GetPosition().y > 500.0f) {
            it = m_Bullets.erase(it);
        } else {
            ++it;
        }
    }

    // --- 1. 生成敵機邏輯 ---
    // 每一幀減少計時器
    m_EnemySpawnTimer -= 1.0f;

    // 當計時器歸零或小於零時，生成一隻新敵機
    if (m_EnemySpawnTimer <= 0.0f) {
        // 隨機產生 X 座標 (假設視窗寬度是 720，範圍抓 -300 到 300)
        float randomX = (rand() % 600) - 300.0f;

        // 從畫面上方 (例如 y = 400) 生成敵機
        auto newEnemy = std::make_shared<Enemy>(glm::vec2(randomX, 400.0f));
        m_Enemies.push_back(newEnemy);

        // 重置計時器 (假設 60 幀約 1 秒，這裡設定約 1 秒生一隻)
        m_EnemySpawnTimer = 60.0f;
    }

    // --- 2. 更新與繪製敵機 ---
    for (auto it = m_Enemies.begin(); it != m_Enemies.end(); ) {
        auto& enemy = *it;
        enemy->Update();
        enemy->Draw();

        // 如果敵機飛出畫面底部 (例如 y < -400)，則從記憶體中移除
        if (enemy->GetPosition().y < -400.0f) {
            it = m_Enemies.erase(it);
        } else {
            ++it;
        }
    }

    // --- 1. 先更新敵機與子彈的位置 (不要在這裡 Draw) ---
    for (auto& enemy : m_Enemies) enemy->Update();
    for (auto& bullet : m_Bullets) bullet->Update();

    // --- 2. 碰撞偵測 (子彈 vs 敵機) ---
    for (auto bulletIt = m_Bullets.begin(); bulletIt != m_Bullets.end(); ) {
        bool bulletHit = false;

        for (auto enemyIt = m_Enemies.begin(); enemyIt != m_Enemies.end(); ) {
            float dist = glm::distance((*bulletIt)->GetPosition(), (*enemyIt)->GetPosition());

            if (dist < 30.0f) { // 擊中敵機
                // --- 新增：機率掉落道具 (例如 20% 機率) ---
                if (std::rand() % 100 < 20) {
                    // 在敵機死亡的位置生成一個武器升級道具
                    auto newItem = std::make_shared<Item>((*enemyIt)->GetPosition(), Item::Type::WEAPON_UPGRADE);
                    m_Items.push_back(newItem);
                }

                enemyIt = m_Enemies.erase(enemyIt);
                bulletHit = true;
                break;
            } else {
                ++enemyIt;
            }
        }

        // --- 3. 檢查子彈是否該被移除 ---
        // 如果打中敵人了，或者飛出畫面頂部了，就把子彈移除
        if (bulletHit || (*bulletIt)->GetPosition().y > 500.0f) {
            bulletIt = m_Bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    // --- 4. 移除飛出畫面底部的敵機 ---
    for (auto enemyIt = m_Enemies.begin(); enemyIt != m_Enemies.end(); ) {
        if ((*enemyIt)->GetPosition().y < -400.0f) {
            enemyIt = m_Enemies.erase(enemyIt);
        } else {
            ++enemyIt;
        }
    }

    // --- 2. 新增：玩家與道具的碰撞偵測 (拾取道具) ---
    for (auto itemIt = m_Items.begin(); itemIt != m_Items.end(); ) {
        (*itemIt)->Update(); // 更新道具位置

        // 計算玩家與道具的距離
        float distToPlayer = glm::distance(m_Player->GetPosition(), (*itemIt)->GetPosition());

        // 假設玩家的拾取半徑是 40.0f
        if (distToPlayer < 40.0f) {
            // 觸發道具效果！
            if ((*itemIt)->GetType() == Item::Type::WEAPON_UPGRADE) {

                // --- 呼叫玩家的升級函式 ---
                m_Player->UpgradeWeapon();

                LOG_INFO("武器升級！目前等級: {}", m_Player->GetWeaponLevel());
            }
            // 拾取後移除道具
            itemIt = m_Items.erase(itemIt);

        } else if ((*itemIt)->GetPosition().y < -400.0f) {
            // 道具飛出畫面底部，沒被撿到，直接移除
            itemIt = m_Items.erase(itemIt);
        } else {
            ++itemIt;
        }
    }

    // --- 5. 統一在這裡繪製所有物件 (確保層級正確) ---
    m_Bg1->Draw();
    m_Bg2->Draw();
    for (auto& enemy : m_Enemies) enemy->Draw();
    for (auto& bullet : m_Bullets) bullet->Draw();
    for (auto& item : m_Items) item->Draw();
    m_Player->Draw();

    /* 原有的退出邏輯 */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
