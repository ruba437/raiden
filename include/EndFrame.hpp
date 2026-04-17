#ifndef ENDFRAME_HPP
#define ENDFRAME_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>

class EndFrame : public Util::GameObject {
public:
    EndFrame() {
        m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/background/EndBackground.png");
        m_ZIndex = 20; // 設高一點，確保蓋在玩家和背景上面
        
        // 放在畫面正中央
        m_Transform.translation = {0.0f, 0.0f}; 
        
        // 如果你的裱框圖檔太小或太大，可以用 scale 放大縮小
        m_Transform.scale = {1.0f, 1.0f};
    }
};

#endif