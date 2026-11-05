// SPDX-License-Identifier: BSD-3-Clause
#include "loupeview.h"

#include "ui/label.h"
#include "ui/painter.h"

#include <cstdio>

using namespace cutefish::ui;

LoupeView::LoupeView(const Bitmap& bmp)
    : m_bmp(bmp)
{
    setName("loupe");
    setGeometry({0, 0, double(kWidth), double(kHeight)});

    // Hex color label
    m_hexLabel = addChild<Label>("#000000");
    m_hexLabel->setFontSize(13);
    m_hexLabel->setWeight(600);
    m_hexLabel->setColor(Color{245, 245, 245, 255});
    m_hexLabel->setGeometry({kMargin + 34, kMargin + kGridSize + 8, double(kWidth - kMargin * 2 - 34), 18});

    // RGB label
    m_rgbLabel = addChild<Label>("RGB: 0, 0, 0");
    m_rgbLabel->setFontSize(10);
    m_rgbLabel->setColor(Color{175, 175, 180, 255});
    m_rgbLabel->setGeometry({kMargin + 34, kMargin + kGridSize + 27, double(kWidth - kMargin * 2 - 34), 14});

    // Helpful interaction hint
    m_hintLabel = addChild<Label>("Click to copy • Esc to cancel");
    m_hintLabel->setFontSize(10);
    m_hintLabel->setColor(Color{145, 145, 150, 255});
    m_hintLabel->setTextAlignment(Label::TextAlignment::Center);
    m_hintLabel->setGeometry({double(kMargin), double(kMargin + kGridSize + 48), double(kWidth - kMargin * 2), 14});
}

void LoupeView::updatePosition(int screenX, int screenY)
{
    m_targetX = screenX;
    m_targetY = screenY;

    uint32_t argb = 0;
    if (m_targetX >= 0 && m_targetX < m_bmp.width() && m_targetY >= 0 && m_targetY < m_bmp.height()) {
        argb = m_bmp.pixel(m_targetX, m_targetY);
    }
    m_currentColor = Color::fromArgb(argb);

    char hexBuf[16];
    snprintf(hexBuf, sizeof(hexBuf), "#%02X%02X%02X", m_currentColor.r, m_currentColor.g, m_currentColor.b);
    m_hexString = hexBuf;
    m_hexLabel->setText(m_hexString);

    char rgbBuf[32];
    snprintf(rgbBuf, sizeof(rgbBuf), "%d, %d, %d", m_currentColor.r, m_currentColor.g, m_currentColor.b);
    m_rgbLabel->setText(rgbBuf);

    requestPaint();
}

void LoupeView::paint(Painter& painter)
{
    RectF r = localRect();

    // 1. Drop shadow for depth
    painter.drawShadow(r, 14, 10, Color{0, 0, 0, 120}, {0, 4});

    // 2. Translucent sleek modern card
    painter.fillRoundedRect(r, 14, Color{24, 24, 28, 245});
    painter.strokeRoundedRect(r, 14, Color{255, 255, 255, 45}, 1);

    // 3. Magnifier grid area
    RectF gridRect{double(kMargin), double(kMargin), double(kGridSize), double(kGridSize)};

    painter.save();
    painter.clipRoundedRect(gridRect, 8);

    const int half = kGridDim / 2;
    for (int dy = -half; dy <= half; ++dy) {
        for (int dx = -half; dx <= half; ++dx) {
            int px = m_targetX + dx;
            int py = m_targetY + dy;
            Color c{0, 0, 0, 255};
            if (px >= 0 && px < m_bmp.width() && py >= 0 && py < m_bmp.height()) {
                c = Color::fromArgb(m_bmp.pixel(px, py));
            }
            RectF cellRect{
                double(kMargin + (dx + half) * kCellSize),
                double(kMargin + (dy + half) * kCellSize),
                double(kCellSize),
                double(kCellSize)
            };
            painter.fillRect(cellRect, c);
        }
    }

    // Grid divider lines
    for (int i = 1; i < kGridDim; ++i) {
        double x = kMargin + i * kCellSize;
        double y = kMargin + i * kCellSize;
        painter.drawVerticalLine(x, kMargin, kGridSize, Color{0, 0, 0, 35}, 1);
        painter.drawHorizontalLine(kMargin, y, kGridSize, Color{0, 0, 0, 35}, 1);
    }

    // Target pixel reticle at center
    double cx = kMargin + half * kCellSize;
    double cy = kMargin + half * kCellSize;
    RectF centerRect{cx, cy, double(kCellSize), double(kCellSize)};
    // Black outer frame
    painter.strokeRoundedRect(RectF{cx - 1, cy - 1, double(kCellSize + 2), double(kCellSize + 2)}, 1, Color{0, 0, 0, 255}, 1);
    // White inner frame
    painter.strokeRoundedRect(centerRect, 1, Color{255, 255, 255, 255}, 1);

    painter.restore();

    // Subtle outline around grid
    painter.strokeRoundedRect(gridRect, 8, Color{255, 255, 255, 30}, 1);

    // 4. Color Swatch
    RectF swatchRect{double(kMargin), double(kMargin + kGridSize + 10), 26, 26};
    painter.fillRoundedRect(swatchRect, 6, m_currentColor);
    painter.strokeRoundedRect(swatchRect, 6, Color{255, 255, 255, 70}, 1);
}
