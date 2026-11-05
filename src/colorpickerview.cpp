// SPDX-License-Identifier: BSD-3-Clause
#include "colorpickerview.h"

#include "ui/application.h"
#include "ui/clipboard.h"
#include "ui/event.h"
#include "ui/keys.h"
#include "ui/painter.h"

#include <algorithm>
#include <cstdio>
#include <iostream>

using namespace cutefish::ui;

ColorPickerView::ColorPickerView(Application& app, const Bitmap& bmp, double scale)
    : m_app(app), m_bmp(bmp), m_scale(scale > 0.0 ? scale : 1.0)
{
    setName("colorPicker");
    setTracksHover(true);
    setCursorShape(CursorShape::Blank);

    m_loupe = addChild<LoupeView>(m_bmp);
    double initialLogicalX = (m_bmp.width() / 2.0) / m_scale;
    double initialLogicalY = (m_bmp.height() / 2.0) / m_scale;
    updateLoupe(initialLogicalX, initialLogicalY);
}

void ColorPickerView::layout()
{
    m_screenW = geometry().width;
    m_screenH = geometry().height;
    if (m_screenW > 0.0 && m_bmp.width() > 0) {
        m_scale = double(m_bmp.width()) / m_screenW;
    }
}

void ColorPickerView::hoverMove(const PointerEvent& event)
{
    updateLoupe(event.position.x, event.position.y);
}

void ColorPickerView::pointerMove(const PointerEvent& event)
{
    updateLoupe(event.position.x, event.position.y);
}

void ColorPickerView::pointerPress(const PointerEvent& event)
{
    if (event.button == MouseButton::Left) {
        copyAndExit();
    } else if (event.button == MouseButton::Right) {
        m_app.quit(0);
    }
}

bool ColorPickerView::keyPress(const KeyEvent& event)
{
    if (event.key == key::Escape) {
        m_app.quit(0);
        return true;
    }
    return false;
}

void ColorPickerView::copyAndExit()
{
    std::string hexStr = m_loupe->hexString();

    // 1. Copy to clipboard and selection via Cutefish Toolkit
    Clipboard::setText(hexStr);
    Clipboard::setText(hexStr, ClipboardMode::Selection);

    // 2. Also ensure wl-copy persists clipboard contents after termination
    if (FILE* fp = popen("wl-copy", "w")) {
        fputs(hexStr.c_str(), fp);
        pclose(fp);
    }

    // 3. Print hex to stdout for easy CLI piping/scripting
    std::cout << hexStr << std::endl;

    m_app.quit(0);
}

void ColorPickerView::updateLoupe(double logicalX, double logicalY)
{
    m_mouseX = logicalX;
    m_mouseY = logicalY;

    // Convert logical coordinates to physical device coordinates for 1:1 pixel sampling
    int devX = std::clamp(static_cast<int>(logicalX * m_scale), 0, m_bmp.width() - 1);
    int devY = std::clamp(static_cast<int>(logicalY * m_scale), 0, m_bmp.height() - 1);
    m_loupe->updatePosition(devX, devY);

    // Intelligent positioning in logical screen space
    double loupeX = logicalX + 24;
    double loupeY = logicalY + 24;

    double maxW = (m_screenW > 0.0) ? m_screenW : (m_bmp.width() / m_scale);
    double maxH = (m_screenH > 0.0) ? m_screenH : (m_bmp.height() / m_scale);

    if (loupeX + LoupeView::kWidth > maxW - 10) {
        loupeX = logicalX - LoupeView::kWidth - 24;
    }
    if (loupeY + LoupeView::kHeight > maxH - 10) {
        loupeY = logicalY - LoupeView::kHeight - 24;
    }

    if (loupeX < 10) loupeX = 10;
    if (loupeY < 10) loupeY = 10;

    m_loupe->setGeometry({loupeX, loupeY, double(LoupeView::kWidth), double(LoupeView::kHeight)});
    requestPaint();
}

void ColorPickerView::paint(Painter& painter)
{
    // Draw smooth crosshair centered on the sampled pixel
    double x = m_mouseX;
    double y = m_mouseY;
    Color crossColor{255, 255, 255, 220};
    Color crossShadow{0, 0, 0, 160};

    // Shadow lines for high visibility on bright backgrounds
    painter.drawHorizontalLine(x - 9, y + 1, 19, crossShadow, 1);
    painter.drawVerticalLine(x + 1, y - 9, 19, crossShadow, 1);

    // Primary crosshair lines
    painter.drawHorizontalLine(x - 9, y, 19, crossColor, 1);
    painter.drawVerticalLine(x, y - 9, 19, crossColor, 1);
}
