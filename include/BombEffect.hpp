#pragma once
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp" // 引入 Animation 標頭檔
#include <vector>
#include <string>

class BombEffect : public Util::GameObject {
private:
    float m_Timer = 0.0f;
    float m_Duration = 50.0f; // 特效存在的總時間 (例如 50 個 frame)
    bool m_IsDone = false;

public:
    BombEffect(const glm::vec2& startPos) {
        // 1. 將所有連續圖片的路徑放入 vector 中
        // (請依照你實際的檔名與數量進行修改)
        std::vector<std::string> frames = {
            RESOURCE_DIR "/Image/bomb/boom1.png",
            RESOURCE_DIR "/Image/bomb/boom2.png",
            RESOURCE_DIR "/Image/bomb/boom3.png",
            RESOURCE_DIR "/Image/bomb/boom4.png"
        };

        // 2. 建立 Animation 物件並設定給 m_Drawable
        // 通常 Util::Animation 的參數為：(圖片陣列, 是否播放, 切換間隔時間, 是否循環播放, 冷卻時間)
        // 這裡設定為 true (播放), 50 (每張圖停留時間), false (不循環，播完一次就好), 0
        auto animation = std::make_shared<Util::Animation>(frames, true, 70, false, 0);
        m_Drawable = animation;
        
        m_Transform.translation = startPos;
        // 如果你的爆炸圖片本身太小，可以在這裡直接給予一個固定的放大倍率
        m_Transform.scale = {2.0f, 2.0f}; 
        m_ZIndex = 8;
    }

    void Update() {
        if (m_IsDone) return;

        m_Timer += 1.0f;
        
        // 注意：Util::Animation 通常會自己在底層處理圖片的切換，
        // 所以我們這裡不需要再去動 scale 或是切換圖片。

        // 當計時器超過我們設定的總時間，就標記為結束，讓 App.cpp 把它清除
        if (m_Timer >= m_Duration) {
            m_IsDone = true;
        }
    }

    bool IsDone() const { return m_IsDone; }
};