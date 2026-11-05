// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include "ui/view.h"
#include "ui/bitmap.h"
#include "loupeview.h"

namespace cutefish::ui {
class Application;
class Painter;
struct PointerEvent;
struct KeyEvent;
}

class ColorPickerView : public cutefish::ui::View {
public:
    ColorPickerView(cutefish::ui::Application& app, const cutefish::ui::Bitmap& bmp, double scale);

    bool acceptsPointer() const override { return true; }
    bool acceptsFocus() const override { return true; }

    void layout() override;
    void hoverMove(const cutefish::ui::PointerEvent& event) override;
    void pointerMove(const cutefish::ui::PointerEvent& event) override;
    void pointerPress(const cutefish::ui::PointerEvent& event) override;
    bool keyPress(const cutefish::ui::KeyEvent& event) override;
    void paint(cutefish::ui::Painter& painter) override;

private:
    void updateLoupe(double logicalX, double logicalY);
    void copyAndExit();

    cutefish::ui::Application& m_app;
    const cutefish::ui::Bitmap& m_bmp;
    LoupeView* m_loupe = nullptr;
    double m_scale = 1.0;
    double m_screenW = 0.0;
    double m_screenH = 0.0;
    double m_mouseX = 0.0;
    double m_mouseY = 0.0;
};
