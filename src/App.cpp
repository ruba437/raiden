#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

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
        // 建立新子彈並放入 vector 中
        auto newBullet = std::make_shared<Bullet>(m_Player->GetPosition());
        m_Bullets.push_back(newBullet); // 現在 m_Bullets 是 vector，所以可以使用 push_back
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
        bool bulletHit = false; // 標記這顆子彈是否打中敵人

        for (auto enemyIt = m_Enemies.begin(); enemyIt != m_Enemies.end(); ) {
            // 計算子彈與敵機之間的距離
            float dist = glm::distance((*bulletIt)->GetPosition(), (*enemyIt)->GetPosition());

            // 碰撞判定半徑 (假設 30.0f，你可以根據圖片大小微調)
            if (dist < 30.0f) {
                // 距離夠近，判定擊中！
                enemyIt = m_Enemies.erase(enemyIt); // 從畫面上移除敵機
                bulletHit = true;                   // 標記子彈已觸發碰撞
                break; // 一顆子彈一次只能打一架敵機，直接跳出內層迴圈
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

    // --- 5. 統一在這裡繪製所有物件 (確保層級正確) ---
    m_Bg1->Draw();
    m_Bg2->Draw();
    for (auto& enemy : m_Enemies) enemy->Draw();
    for (auto& bullet : m_Bullets) bullet->Draw();
    m_Player->Draw();

    /* 原有的退出邏輯 */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
