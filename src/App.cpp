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

    float speed = 5.0f; // 每一幀移動的像素
    glm::vec2 dir = {0.0f, 0.0f};

    // 使用 Util::Input 偵測按鍵
    if (Util::Input::IsKeyPressed(Util::Keycode::W)) dir.y += 1.0f;
    if (Util::Input::IsKeyPressed(Util::Keycode::S)) dir.y -= 1.0f;
    if (Util::Input::IsKeyPressed(Util::Keycode::A)) dir.x -= 1.0f;
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) dir.x += 1.0f;

    // 如果有移動輸入，則更新玩家位置
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

    // 繪製玩家
    m_Player->Draw();

    /* 原有的退出邏輯 */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
