#pragma once
#include "Skin.h"
#include <memory>
#include <vector>

namespace OpenFlow::UI::Controls {

class StyleManager {
public:
    static StyleManager& instance() {
        static StyleManager inst;
        return inst;
    }

    void setSkin(std::shared_ptr<ISkin> skin) {
        _currentSkin = skin;
    }

    std::shared_ptr<ISkin> skin() const {
        return _currentSkin;
    }

private:
    StyleManager() {
        _currentSkin = std::make_shared<ModernSkin>();
    }

    std::shared_ptr<ISkin> _currentSkin;
};

} // namespace OpenFlow::UI::Controls
