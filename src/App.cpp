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


    // ==========================================
    // 關卡設計 (Level Design) 初始化
    // ==========================================
    m_LevelTimer = 0.0f;
    m_CurrentEventIndex = 0;

    // 假設遊戲執行速度為 60 FPS (1 秒 = 60.0f 幀)
    // 注意：請務必按照 spawnTime 「由小到大」的順序來排，時間才會正確觸發！
    m_LevelEvents = {
        // { 觸發幀數, { X座標, Y座標 } }
        { 60.0f,   {   0.0f, 450.0f } },  // 1 秒：正中央出一隻
        { 120.0f,  {-150.0f, 450.0f } },  // 2 秒：左邊出一隻
        { 150.0f,  { 150.0f, 450.0f } },  // 2.5 秒：右邊出一隻

        // 4 秒時：左右「同時」各出一隻 (時間設一樣即可)
        { 240.0f,  {-250.0f, 450.0f } },
        { 240.0f,  { 250.0f, 450.0f } },

        // 6 秒時：排成 V 字型的三隻敵機陣型
        { 360.0f,  {-150.0f, 450.0f } },
        { 370.0f,  {   0.0f, 450.0f } },
        { 380.0f,  { 150.0f, 450.0f } },

        // ... 你可以無限往下編排整個關卡 ...
    };

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    float bgSpeed = 0.5f;

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

    if (m_PlayerShootTimer > 0.0f) {
        m_PlayerShootTimer -= 1.0f;
    }

    if (m_MissileShootTimer > 0.0f) {
        m_MissileShootTimer -= 1.0f;
    }

    // --- 2. 偵測攻擊輸入 ---
    if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
        glm::vec2 playerPos = m_Player->GetPosition();
        if (m_PlayerShootTimer <= 0.0f) {
            int mainlevel = m_Player->GetWeaponLevel();
            auto weaponType = m_Player->GetWeaponType();

            if (weaponType == Player::WeaponType::DEFAULT) {
                // ==========================================
                // 散彈 (DEFAULT) 邏輯
                // ==========================================
                if (mainlevel == 1) {
                    m_Bullets.push_back(std::make_shared<Bullet>(playerPos));
                } else if (mainlevel == 2) {
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 15.0f, playerPos.y}));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 15.0f, playerPos.y}));
                } else if (mainlevel == 3) {
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 10.0f, playerPos.y}, glm::vec2(0.0f, 10.0f)));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 10.0f, playerPos.y}, glm::vec2(0.0f, 10.0f)));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 25.0f, playerPos.y - 10.0f}, glm::vec2(-3.0f, 10.0f)));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 25.0f, playerPos.y - 10.0f}, glm::vec2(3.0f, 10.0f)));
                }
                else if (mainlevel >= 4) {
                    // --- 使用資料驅動的方式來產生大量子彈 ---

                    // 建立一個結構來儲存「一個發射方向」的基礎偏移與速度
                    struct Stream { glm::vec2 posOffset; glm::vec2 velocity; };
                    std::vector<Stream> streams;

                    int parallelCount = 1; // 每個方向要並排幾顆子彈
                    float spacing = 8.0f;  // 並排子彈之間的距離

                    if (mainlevel == 4) {
                        // 等級 4：5 個方向，每個方向並排 2 顆 (共 10 顆)
                        parallelCount = 2;
                        streams = {
                            {{0.0f, 0.0f},     {0.0f, 10.0f}},   // 正中
                            {{-15.0f, -5.0f},  {-2.0f, 10.0f}},  // 內左
                            {{15.0f, -5.0f},   {2.0f, 10.0f}},   // 內右
                            {{-30.0f, -10.0f}, {-4.0f, 10.0f}},  // 外左
                            {{30.0f, -10.0f},  {4.0f, 10.0f}}    // 外右
                        };
                    } else if (mainlevel >= 5) {
                        // 等級 5：7 個方向，每個方向並排 3 顆 (共 21 顆，極密集彈幕！)
                        parallelCount = 3;
                        spacing = 6.0f; // 數量多，間距稍微縮小一點
                        streams = {
                            {{0.0f, 0.0f},     {0.0f, 10.0f}},
                            {{-12.0f, -5.0f},  {-1.5f, 10.0f}},
                            {{12.0f, -5.0f},   {1.5f, 10.0f}},
                            {{-24.0f, -10.0f}, {-3.5f, 10.0f}},
                            {{24.0f, -10.0f},  {3.5f, 10.0f}},
                            {{-36.0f, -15.0f}, {-6.0f, 10.0f}},
                            {{36.0f, -15.0f},  {6.0f, 10.0f}}
                        };
                    }

                    // 雙層迴圈：對每一個方向 (Stream)，產生 N 顆並排的子彈
                    for (const auto& stream : streams) {
                        for (int i = 0; i < parallelCount; ++i) {

                            // 計算並排時的 X 軸置中偏移量
                            // 例如排 3 顆，間距 6，則偏移依序為 -6, 0, 6
                            float startX = -(parallelCount - 1) * spacing / 2.0f;
                            float currentOffsetX = startX + (i * spacing);

                            // 最終位置 = 玩家位置 + 該方向的基礎偏移 + 並排的微調偏移
                            glm::vec2 finalPos = {
                                playerPos.x + stream.posOffset.x + currentOffsetX,
                                playerPos.y + stream.posOffset.y
                            };

                            m_Bullets.push_back(std::make_shared<Bullet>(finalPos, stream.velocity));
                        }
                    }
                }
            }
            else if (weaponType == Player::WeaponType::LASER) {
                // ==========================================
                // 新增的雷射 (LASER) 邏輯
                // 特色：速度極快 (Velocity Y = 25.0f)，筆直向前，使用 laser.png
                // ==========================================
                if (mainlevel == 1) {
                    m_Bullets.push_back(std::make_shared<Bullet>(playerPos, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                } else if (mainlevel == 2) {
                    // 等級 2：兩道極近的雷射並排
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 8.0f, playerPos.y}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 8.0f, playerPos.y}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                } else if (mainlevel == 3) {
                    // 等級 3：三道雷射並排，中間稍微往前凸出
                    m_Bullets.push_back(std::make_shared<Bullet>(playerPos, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 16.0f, playerPos.y - 5.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 16.0f, playerPos.y - 5.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                } else if (mainlevel == 4) {
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 12.0f, playerPos.y}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 12.0f, playerPos.y}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 28.0f, playerPos.y - 10.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 28.0f, playerPos.y - 10.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                } else if (mainlevel >= 5) {
                    m_Bullets.push_back(std::make_shared<Bullet>(playerPos, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 18.0f, playerPos.y - 5.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 18.0f, playerPos.y - 5.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 36.0f, playerPos.y - 10.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                    m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 36.0f, playerPos.y - 10.0f}, glm::vec2(0.0f, 25.0f), RESOURCE_DIR "/Image/bullet/laser_attack.png"));
                }
            }
            if (weaponType == Player::WeaponType::LASER) {
                m_PlayerShootTimer = 5.0f;  // 雷射極快
            } else {
                m_PlayerShootTimer = 10.0f; // 散彈中等
            }
        }
        // ==========================================
    // 系統 B：發射副武器 (飛彈) - 與主武器並存
    // ==========================================
    int missileLevel = m_Player->GetMissileLevel();

    // 只要等級大於 0，就代表有飛彈系統，跟主武器一起發射
    if (missileLevel > 0 && m_MissileShootTimer <= 0.0f) {
        glm::vec2 accel = {0.0f, 0.1f};
        std::string imgPath = RESOURCE_DIR "/Image/bullet/missile.png";
        if (missileLevel == 1) {
            // 等級 1：兩枚飛彈，剛發射時稍微往外拋 (X 軸初速為 -2.0f 和 2.0f)
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 20.0f, playerPos.y}, glm::vec2(-2.0f, 1.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 20.0f, playerPos.y}, glm::vec2(2.0f, 1.0f), imgPath, accel));
        }
        else if (missileLevel == 2) {
            // 等級 2：四枚飛彈
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 15.0f, playerPos.y}, glm::vec2(-1.0f, 2.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 15.0f, playerPos.y}, glm::vec2(1.0f, 2.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 30.0f, playerPos.y - 10.0f}, glm::vec2(-3.0f, 0.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 30.0f, playerPos.y - 10.0f}, glm::vec2(3.0f, 0.0f), imgPath, accel));
        }
        else if (missileLevel >= 3) {
            // 等級 3：四枚飛彈，更誇張的向外拋出軌跡
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 15.0f, playerPos.y}, glm::vec2(-1.5f, 2.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 15.0f, playerPos.y}, glm::vec2(1.5f, 2.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 30.0f, playerPos.y - 10.0f}, glm::vec2(-3.0f, 0.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 30.0f, playerPos.y - 10.0f}, glm::vec2(3.0f, 0.0f), imgPath, accel));
            // 外側飛彈初速甚至帶有一點往後掉的感覺 (Y = -2.0f)，然後才往前衝刺
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x - 35.0f, playerPos.y - 10.0f}, glm::vec2(-4.0f, -2.0f), imgPath, accel));
            m_Bullets.push_back(std::make_shared<Bullet>(glm::vec2{playerPos.x + 35.0f, playerPos.y - 10.0f}, glm::vec2(4.0f, -2.0f), imgPath, accel));
        }
        m_MissileShootTimer = 60.0f;
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
    // ==========================================
    // 關卡時間軸系統 (Level Timeline)
    // ==========================================
    m_LevelTimer += 1.0f; // 每一幀時間 +1

    // 使用 while 迴圈檢查：如果當前時間大於等於清單上要求的時間，就生成敵人
    // (用 while 是因為同一幀可能會「同時」生成好幾隻敵人，例如上面的 240.0f)
    while (m_CurrentEventIndex < m_LevelEvents.size() &&
           m_LevelTimer >= m_LevelEvents[m_CurrentEventIndex].spawnTime) {

        // 取得當前事件設定的座標
        glm::vec2 spawnPos = m_LevelEvents[m_CurrentEventIndex].position;

        // 產生敵機 (血量目前預設為 5)
        m_Enemies.push_back(std::make_shared<Enemy>(spawnPos, 5));

        // 推進到下一個事件
        m_CurrentEventIndex++;
    }



    // --- 1. 先更新敵機與子彈的位置 (不要在這裡 Draw) ---
    glm::vec2 playerPos = m_Player->GetPosition(); // 先取得玩家位置

    for (auto& enemy : m_Enemies) {
        // --- 呼叫 Update 時，將 playerPos 傳進去 ---
        enemy->Update(playerPos);

        // 敵機發射子彈邏輯
        if (enemy->ReadyToShoot()) {
            auto enemyBullet = std::make_shared<Bullet>(
                enemy->GetPosition(),
                glm::vec2(0.0f, -8.0f),
                RESOURCE_DIR "/Image/bullet/enemy_attack_1.png"
            );
            m_EnemyBullets.push_back(enemyBullet);
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

            if (distToEnemy < 30.0f) {

                // --- 1. 子彈對敵人造成傷害 (假設每顆子彈傷害為 1) ---
                (*enemyIt)->TakeDamage(1);
                bulletHit = true; // 子彈本身還是會消失 (貫穿雷射除外)

                // --- 2. 檢查敵人是否死亡 ---
                if ((*enemyIt)->IsDead()) {
                    m_Score += 100;
                    m_ScoreUI->UpdateScore(m_Score);

                    // 掉落道具邏輯 (保留你原本的機率判斷)
                    int dropChance = std::rand() % 100;
                    if (dropChance < 50) {
                        m_Items.push_back(std::make_shared<Item>((*enemyIt)->GetPosition(), Item::Type::WEAPON_UPGRADE));
                    }
                    if (dropChance < 30) {
                        m_Items.push_back(std::make_shared<Item>((*enemyIt)->GetPosition(), Item::Type::SCORE_BONUS));
                    }

                    // 敵人死亡，從畫面上移除
                    enemyIt = m_Enemies.erase(enemyIt);
                } else {
                    // 如果敵人還沒死，就不 erase，指標繼續往下走
                    ++enemyIt;
                }

                break; // 跳出內層迴圈 (這顆子彈已經打中目標，不需要再檢查其他敵人)
            } else {
                ++enemyIt;
            }
        }

        glm::vec2 bPos = (*bulletIt)->GetPosition();

        bool isOutOfBounds = (bPos.x < -305.0f || bPos.x > 305.0f ||
                              bPos.y < -450.0f || bPos.y > 450.0f);

        // 如果打中敵機，或者飛出邊界，就將這顆子彈移除
        if (bulletHit || isOutOfBounds) {
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

        if (distToPlayer < 40.0f) {
            if ((*itemIt)->GetType() == Item::Type::WEAPON_UPGRADE) {
                m_Player->ChangeWeapon(Player::WeaponType::DEFAULT);
                LOG_INFO("主武器：切換為散彈！");
            }
            else if ((*itemIt)->GetType() == Item::Type::WEAPON_LASER) {
                m_Player->ChangeWeapon(Player::WeaponType::LASER);
                LOG_INFO("主武器：切換為雷射！");
            }
            else if ((*itemIt)->GetType() == Item::Type::WEAPON_MISSILE) {
                // --- 修改這裡：獨立升級副武器，不影響主武器 ---
                m_Player->UpgradeMissile();
                LOG_INFO("副武器：飛彈升級！目前等級: {}", m_Player->GetMissileLevel());
            }
            else if ((*itemIt)->GetType() == Item::Type::SCORE_BONUS) {
                m_Score += 1000;
                m_ScoreUI->UpdateScore(m_Score);
            }
            itemIt = m_Items.erase(itemIt);
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

            bulletIt = m_EnemyBullets.erase(bulletIt);

            if (m_Player->IsDead()) {
                LOG_INFO("💀 玩家血量歸零，遊戲結束！");
                m_CurrentState = State::END;
                break;
            }
        }
        else {
            // --- 敵機子彈的全方位邊界檢查 ---
            glm::vec2 bPos = (*bulletIt)->GetPosition();
            bool isOutOfBounds = (bPos.x < -305.0f || bPos.x > 305.0f ||
                                  bPos.y < -450.0f || bPos.y > 450.0f);

            if (isOutOfBounds) {
                bulletIt = m_EnemyBullets.erase(bulletIt);
            } else {
                ++bulletIt;
            }
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
