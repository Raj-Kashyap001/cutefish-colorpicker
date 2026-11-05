# Cutefish Color Picker (CLI)

A simple, fast, and straightforward screen color picker CLI application for Cutefish OS built with modern C++20 and **cutefish-toolkit**.

## Features

- **Pixel-Perfect 1:1 Rendering (No Zooming)**: Automatically detects Cutefish display scaling (e.g. 1.25× fractional scaling) so the screen capture matches your actual desktop 1:1 without any distortion or zooming.
- **Direct & Silent Screen Capture**: Uses Cutefish Compositor DBus interface (`org.cutefish.Screenshot.captureOutput`) to grab the screen silently with **zero notifications** or camera shutter noises.
- **Precision Magnifier (Loupe)**:
  - 11×11 pixel inspection window magnified at 12× per pixel.
  - High-contrast target reticle around the sampled center pixel.
  - Live color preview swatch.
  - Real-time HEX and RGB value readout.
  - Intelligent screen edge detection to ensure the loupe never leaves the screen or obstructs the sample.
- **Straightforward Clipboard Copying**:
  - **Left Click**: Samples the pixel, copies HEX (`#RRGGBB`) to clipboard (both standard clipboard and primary selection), outputs the HEX code to standard output, and exits immediately.
  - **Right Click** or **Escape**: Cancels and exits cleanly.
- **CLI Only**: Installed as a command-line tool without cluttering the application menu.

## Usage

Simply run:
```bash
cutefish-colorpicker
```

Or capture the output into a variable in scripts:
```bash
COLOR=$(cutefish-colorpicker)
echo "Selected color: $COLOR"
```

## Building & Installing

### Dependencies

- `cutefish-toolkit`
- `wl-clipboard`
- `cmake`
- `g++` (C++20)

### Build with CMake

```bash
cd cutefish-colorpicker
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```
