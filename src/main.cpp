// SPDX-License-Identifier: BSD-3-Clause
#include "colorpickerview.h"

#include "base/dbus.h"
#include "ui/application.h"
#include "ui/image_view.h"
#include "ui/keys.h"
#include "ui/window.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifndef COLORPICKER_VERSION
#define COLORPICKER_VERSION "1.0"
#endif

using namespace cutefish;

class RootContainer : public ui::View {
public:
    void layout() override {
        for (const auto& child : children()) {
            child->setGeometry(localRect());
        }
    }
};

static void printHelp(const char* prog)
{
    std::printf("Usage: %s [options]\n"
                "A fast screen color picker CLI for Cutefish using cutefish-toolkit.\n\n"
                "Options:\n"
                "  -h, --help     Displays this help message.\n"
                "  -v, --version  Displays version information.\n\n"
                "Controls:\n"
                "  Left Click     Pick color, copy HEX to clipboard, output to stdout, and exit.\n"
                "  Right Click    Cancel and exit.\n"
                "  Escape         Cancel and exit.\n",
                prog);
}

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help" || arg == "-?") {
            printHelp(argv[0]);
            return 0;
        }
        if (arg == "-v" || arg == "--version") {
            std::printf("cutefish-colorpicker %s\n", COLORPICKER_VERSION);
            return 0;
        }
    }

    ui::Application app(argc, argv);
    app.setApplicationId("cutefish-colorpicker");

    // 1. Silent screen capture via Cutefish Compositor DBus interface (zero notification)
    auto conn = base::dbus::Connection::session();
    if (!conn) {
        std::fprintf(stderr, "Error: Unable to connect to DBus session bus.\n");
        return 1;
    }

    auto msg = conn->newMethodCall("org.cutefish.Screenshot", "/Screenshot", "org.cutefish.Screenshot", "captureOutput");
    msg.append(std::string(""), false);
    auto reply = conn->call(msg);
    if (reply.isError()) {
        std::fprintf(stderr, "Error capturing screen: %s\n", reply.error().message.c_str());
        return 1;
    }

    std::vector<uint8_t> pngBytes;
    if (!reply.read(pngBytes) || pngBytes.empty()) {
        std::fprintf(stderr, "Error: Received empty screen capture.\n");
        return 1;
    }

    std::string_view sv(reinterpret_cast<const char*>(pngBytes.data()), pngBytes.size());
    auto bmp = ui::Bitmap::decode(sv);
    if (bmp.isNull()) {
        std::fprintf(stderr, "Error: Failed to decode screen capture bitmap.\n");
        return 1;
    }

    // Determine screen scale and logical geometry to ensure 1:1 pixel match (no zoom)
    auto screens = app.screens();
    double scale = 1.0;
    double screenW = 0.0;
    double screenH = 0.0;

    if (!screens.empty()) {
        scale = screens[0].scale;
        screenW = screens[0].geometry.width;
        screenH = screens[0].geometry.height;
    }

    if (scale <= 0.0)
        scale = 1.0;
    if (screenW <= 0.0)
        screenW = bmp.width() / scale;
    if (screenH <= 0.0)
        screenH = bmp.height() / scale;

    // 2. Fullscreen overlay window
    ui::Window win;
    win.setTitle("Color Picker");
    win.setFullScreen(true);

    auto container = std::make_unique<RootContainer>();
    container->setGeometry({0, 0, screenW, screenH});

    // Background frozen screenshot view with matching devicePixelRatio (1:1 unscaled)
    auto imgView = container->addChild<ui::ImageView>();
    imgView->setGeometry({0, 0, screenW, screenH});
    imgView->setPixels(bmp.width(), bmp.height(), reinterpret_cast<const uint32_t*>(bmp.bits()), bmp.stride(), true, scale);

    // Interactive picker and loupe layer
    auto picker = container->addChild<ColorPickerView>(app, bmp, scale);
    picker->setGeometry({0, 0, screenW, screenH});

    // Escape shortcut fallback
    win.addShortcut(ui::NoModifier, ui::key::Escape, [&app]() {
        app.quit(0);
    });

    win.setContentView(std::move(container));
    win.show();

    return app.run();
}
