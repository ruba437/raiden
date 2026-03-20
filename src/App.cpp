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

    m_Score = 0; // 遊戲開始分數歸零
    m_ScoreUI = std::make_shared<ScoreUI>();

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
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        glm::vec2 playerPos = m_Player->GetPosition();
        int level = m_Player->GetWeaponLevel();

        if (level == 1) {
            // 等級 1：單發正中央 (使用預設速度)
            m_Bullets.push_back(std::make_shared<Bullet>(playerPos));

        } else if (level == 2) {
            // 等級 2：雙排直飛
            glm::vec2 leftPos = {playerPos.x - 15.0f, playerPos.y};
            glm::vec2 rightPos = {playerPos.x + 15.0f, playerPos.y};

            m_Bullets.push_back(std::make_shared<Bullet>(leftPos));
            m_Bullets.push_back(std::make_shared<Bullet>(rightPos));

        } else if (level >= 3) {
            // 等級 3：中間兩排直飛 + 左右兩側斜向散彈

            // 1. 中間兩顆 (直飛)
            glm::vec2 innerLeftPos = {playerPos.x - 10.0f, playerPos.y};
            glm::vec2 innerRightPos = {playerPos.x + 10.0f, playerPos.y};
            m_Bullets.push_back(std::make_shared<Bullet>(innerLeftPos, glm::vec2(0.0f, 10.0f)));
            m_Bullets.push_back(std::make_shared<Bullet>(innerRightPos, glm::vec2(0.0f, 10.0f)));

            // 2. 左右外側兩顆 (斜飛)
            glm::vec2 outerLeftPos = {playerPos.x - 25.0f, playerPos.y - 10.0f};
            glm::vec2 outerRightPos = {playerPos.x + 25.0f, playerPos.y - 10.0f};

            // 給予斜向的 Velocity，例如 X 軸向外擴散 3.0f，Y 軸向上 10.0f
            m_Bullets.push_back(std::make_shared<Bullet>(outerLeftPos, glm::vec2(-3.0f, 10.0f)));
            m_Bullets.push_back(std::make_shared<Bullet>(outerRightPos, glm::vec2(3.0f, 10.0f)));
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
    m_EnemySpawnTimer -= 2.0f;

    // 當計時器歸零或小於零時，生成一隻新敵機
    if (m_EnemySpawnTimer <= 0.0f) {
        // 隨機產生 X 座標 (假設視窗寬度是 720，範圍抓 -300 到 300)
        float randomX = (rand() % 600) - 300.0f;

        // 從畫面上方 (例如 y = 400) 生成敵機
        auto newEnemy = std::make_shared<Enemy>(glm::vec2(randomX, 400.0f));
        m_Enemies.push_back(newEnemy);

        // 重置計時器 (假設 60 幀約 1 秒，這裡設定約 1 秒生一隻)
        m_EnemySpawnTimer = 120.0f;
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
    for (auto& enemy : m_Enemies) {
        enemy->Update();

        // --- 新增：敵機發射子彈邏輯 ---
        if (enemy->ReadyToShoot()) {
            // 產生一顆向下飛的子彈 (Velocity Y 為負數，例如 -8.0f)
            // 我們在這裡多傳入一個參數：敵機子彈的圖片路徑
            auto enemyBullet = std::make_shared<Bullet>(
                enemy->GetPosition(),
                glm::vec2(0.0f, -8.0f),
                RESOURCE_DIR "/Image/bullet/enemy_attack_1.png"
            );
            m_EnemyBullets.push_back(enemyBullet);

            // 重置該敵機的計時器
            enemy->ResetShootTimer();
        }
    }
    for (auto& bullet : m_Bullets) bullet->Update();
    for (auto& bullet : m_EnemyBullets) bullet->Update(); // <-- 新增：更新敵機子彈位置
    for (auto& item : m_Items) item->Update();


    // ==========================================
    // 2. 碰撞偵測：子彈 vs 敵機
    // ==========================================
    for (auto bulletIt = m_Bullets.begin(); bulletIt != m_Bullets.end(); ) {
        bool bulletHit = false;

        for (auto enemyIt = m_Enemies.begin(); enemyIt != m_Enemies.end(); ) {
            // 計算子彈與敵機的距離
            float distToEnemy = glm::distance((*bulletIt)->GetPosition(), (*enemyIt)->GetPosition());

            if (distToEnemy < 30.0f) { // 子彈擊中敵機
                m_Score += 100;
                m_ScoreUI->UpdateScore(m_Score); // 更新 UI 顯示

                // 掉落道具機率
                if (std::rand() % 100 < 50) {
                    auto newItem = std::make_shared<Item>((*enemyIt)->GetPosition(), Item::Type::WEAPON_UPGRADE);
                    m_Items.push_back(newItem);
                }
                // 移除敵機
                enemyIt = m_Enemies.erase(enemyIt);
                bulletHit = true;
                break;
            } else {
                ++enemyIt;
            }
        }

        // 檢查子彈是否擊中或飛出畫面，若是則移除
        if (bulletHit || (*bulletIt)->GetPosition().y > 500.0f) {
            bulletIt = m_Bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    // ==========================================
    // 3. 碰撞偵測：玩家 vs 敵機
    // ==========================================
    for (auto enemyIt = m_Enemies.begin(); enemyIt != m_Enemies.end(); ) {
        // 計算玩家與敵機的距離
        float distToPlayer = glm::distance(m_Player->GetPosition(), (*enemyIt)->GetPosition());

        if (distToPlayer < 35.0f) { // 玩家撞到敵機
            m_Player->TakeDamage(1);
            LOG_INFO("💥 撞擊敵機！玩家扣血，剩餘血量: {}", m_Player->GetHP());

            enemyIt = m_Enemies.erase(enemyIt); // 敵機撞毀

            // 檢查玩家是否死亡
            if (m_Player->IsDead()) {
                LOG_INFO("💀 玩家血量歸零，遊戲結束！");
                m_CurrentState = State::END;
                break;
            }
            continue;
        }

        // 移除飛出畫面底部的敵機
        if ((*enemyIt)->GetPosition().y < -400.0f) {
            enemyIt = m_Enemies.erase(enemyIt);
        } else {
            ++enemyIt;
        }
    }

    // ==========================================
    // 4. 碰撞偵測：玩家 vs 道具 (拾取道具)
    // ==========================================
    for (auto itemIt = m_Items.begin(); itemIt != m_Items.end(); ) {
        float distToPlayer = glm::distance(m_Player->GetPosition(), (*itemIt)->GetPosition());

        if (distToPlayer < 40.0f) { // 玩家吃到道具

            // 根據吃到的道具種類，執行不同效果
            if ((*itemIt)->GetType() == Item::Type::WEAPON_UPGRADE) {
                m_Player->UpgradeWeapon();
                LOG_INFO("火力升級！目前等級: {}", m_Player->GetWeaponLevel());
            }
            else if ((*itemIt)->GetType() == Item::Type::SCORE_BONUS) {
                m_Score += 1000;
                m_ScoreUI->UpdateScore(m_Score); // 更新 UI 顯示
                LOG_INFO("獲得分數加成！");
            }

            itemIt = m_Items.erase(itemIt); // 拾取後移除道具
        }
        else if ((*itemIt)->GetPosition().y < -400.0f) {
            itemIt = m_Items.erase(itemIt); // 道具飛出畫面底部移除
        }
        else {
            ++itemIt;
        }
    }

    // ==========================================
    // 4.5 新增：碰撞偵測：敵機子彈 vs 玩家
    // ==========================================
    for (auto bulletIt = m_EnemyBullets.begin(); bulletIt != m_EnemyBullets.end(); ) {
        // 計算敵機子彈與玩家的距離
        float distToPlayer = glm::distance(m_Player->GetPosition(), (*bulletIt)->GetPosition());

        // 子彈的判定半徑通常比較小 (例如 20.0f)，這樣玩家比較好閃躲
        if (distToPlayer < 20.0f) {
            m_Player->TakeDamage(1);
            LOG_INFO("💥 被敵機子彈擊中！剩餘血量: {}", m_Player->GetHP());

            bulletIt = m_EnemyBullets.erase(bulletIt); // 移除這顆子彈

            if (m_Player->IsDead()) {
                LOG_INFO("💀 玩家血量歸零，遊戲結束！");
                m_CurrentState = State::END;
                break;
            }
        }
        else if ((*bulletIt)->GetPosition().y < -400.0f) {
            // 敵機子彈飛出畫面底部，移除
            bulletIt = m_EnemyBullets.erase(bulletIt);
        }
        else {
            ++bulletIt;
        }
    }

    // --- 5. 統一在這裡繪製所有物件 (確保層級正確) ---
    m_Bg1->Draw();
    m_Bg2->Draw();
    for (auto& enemy : m_Enemies) enemy->Draw();
    for (auto& item : m_Items) item->Draw();
    for (auto& bullet : m_Bullets) bullet->Draw();
    for (auto& bullet : m_EnemyBullets) bullet->Draw();
    m_Player->Draw();
    m_ScoreUI->Draw();

    /* 原有的退出邏輯 */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
