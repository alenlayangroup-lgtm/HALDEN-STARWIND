# Building HALDEN STARWIND on macOS

## Requirements
- macOS
- Xcode and Command Line Tools
- CMake 3.22+
- Git
- Internet access for the initial JUCE FetchContent download

## Configure
```bash
cmake -S . -B build -G Xcode -DCMAKE_BUILD_TYPE=Release
```

## Build AU and VST3
```bash
cmake --build build --config Release --target HALDEN_STARWIND_AU HALDEN_STARWIND_VST3
```

The resulting plugin bundles are produced under the build tree in the corresponding JUCE target directories. Validate the AU with Apple's auval and test both formats in a DAW.

## Current prototype limitations
The first commit uses JUCE's stock Reverb as a temporary audio-path smoke test. It is not the final STARWIND algorithm. De-Esser, pre-chorus, bespoke FDN Hall, Breeze, Stardust, Silk Tail, Post EQ, and robust Freeze/Infinite behavior remain scheduled DSP milestones. Do not market this prototype as a finished product.
