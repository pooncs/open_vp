#pragma once
#include <QColor>
#include <QFont>

namespace OpenFlow::UI::Controls {

struct SkinColors {
    QColor background;
    QColor border;
    QColor foreground; // Text/Tick marks
    QColor accent;     // Active elements (needles, fills)
    QColor highlight;  // Hover/Focus
    QColor shadow;
};

class ISkin {
public:
    virtual ~ISkin() = default;
    
    virtual QString name() const = 0;
    virtual SkinColors colors() const = 0;
    virtual QFont font() const = 0;
    
    // Geometry constants
    virtual int borderThickness() const { return 1; }
    virtual int cornerRadius() const { return 0; }
};

class ModernSkin : public ISkin {
public:
    QString name() const override { return "Modern"; }
    SkinColors colors() const override {
        return {
            QColor("#f0f0f0"), // Background
            QColor("#cccccc"), // Border
            QColor("#333333"), // Foreground
            QColor("#0078d7"), // Accent (Blue)
            QColor("#e5f1fb"), // Highlight
            QColor(0,0,0,20)   // Shadow
        };
    }
    QFont font() const override { return QFont("Segoe UI", 9); }
    int cornerRadius() const override { return 4; }
};

class DarkSkin : public ISkin {
public:
    QString name() const override { return "Dark"; }
    SkinColors colors() const override {
        return {
            QColor("#2d2d2d"), // Background
            QColor("#454545"), // Border
            QColor("#e0e0e0"), // Foreground
            QColor("#00cc6a"), // Accent (Green)
            QColor("#3e3e42"), // Highlight
            QColor(0,0,0,100)  // Shadow
        };
    }
    QFont font() const override { return QFont("Segoe UI", 9); }
    int cornerRadius() const override { return 4; }
};

class SilverSkin : public ISkin {
public:
    QString name() const override { return "Silver"; }
    SkinColors colors() const override {
        return {
            QColor("#e0e0e0"), // Gradient Start
            QColor("#999999"), // Border
            QColor("#000000"), // Foreground
            QColor("#ff9900"), // Accent (Orange)
            QColor("#ffffff"), // Highlight
            QColor(0,0,0,50)   // Shadow
        };
    }
    QFont font() const override { return QFont("Arial", 9, QFont::Bold); }
    int borderThickness() const override { return 2; }
    int cornerRadius() const override { return 2; }
};

} // namespace OpenFlow::UI::Controls
