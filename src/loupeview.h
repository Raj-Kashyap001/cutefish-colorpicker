// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include "ui/view.h"
#include "ui/bitmap.h"
#include "ui/geometry.h"

#include <string>

namespace cutefish::ui {
class Label;
class Painter;
}

class LoupeView : public cutefish::ui::View {
public:
    static constexpr int kGridDim = 11;      // 11x11 pixel inspection window
    static constexpr int kCellSize = 12;     // 12px per magnified pixel
    static constexpr int kGridSize = kGridDim * kCellSize; // 132x132
    static constexpr int kMargin = 12;
    static constexpr int kWidth = kGridSize + kMargin * 2; // 156
    static constexpr int kHeight = kGridSize + 82;         // 214

    explicit LoupeView(const cutefish::ui::Bitmap& bmp);

    void updatePosition(int screenX, int screenY);
    cutefish::ui::Color currentColor() const { return m_currentColor; }
    std::string hexString() const { return m_hexString; }

    void paint(cutefish::ui::Painter& painter) override;

private:
    const cutefish::ui::Bitmap& m_bmp;
    int m_targetX = 0;
    int m_targetY = 0;
    cutefish::ui::Color m_currentColor{0, 0, 0, 255};
    std::string m_hexString = "#000000";

    cutefish::ui::Label* m_hexLabel = nullptr;
    cutefish::ui::Label* m_rgbLabel = nullptr;
    cutefish::ui::Label* m_hintLabel = nullptr;
};
