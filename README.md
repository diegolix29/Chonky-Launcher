# ChonkyStation Launcher

A Simple GUI launcher for ChonkyStation.exe that provides an easy way to launch games without using the command line.

## Features

- **Path Configuration**: Select ChonkyStation executable and games folder through GUI dialogs
- **Automatic Game Detection**: Scans folders recursively for EBOOT.BIN and ELF files
- **Game List**: Displays all found games in an organized list
- **Easy Launching**: Double-click or select and click launch to start games
- **Persistent Settings**: Saves your paths and preferences between sessions
- **Error Handling**: Comprehensive error messages for common issues

## Requirements

- Qt6 (Core and Widgets modules)
- CMake 3.16 or higher
- C++17 compatible compiler

## Building

### Windows (Recommended)

1. **Install Qt6**:
   ```bash
   # Using vcpkg (recommended)
   vcpkg install qt6-base:x64-windows
   
   # Or download from Qt website
   # https://www.qt.io/download
   ```

2. **Configure and Build**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

3. **Run**:
   ```bash
   # The executable will be in build/Release/ChonkyLauncher.exe
   ./Release/ChonkyLauncher.exe
   ```

### Linux

1. **Install Qt6**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install qt6-base-dev cmake build-essential
   
   # Fedora
   sudo dnf install qt6-qtbase-devel cmake gcc-c++
   ```

2. **Build**:
   ```bash
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

3. **Run**:
   ```bash
   ./ChonkyLauncher
   ```

## Usage

1. **First Launch**:
   - Click "Browse..." next to "ChonkyStation Executable" to select your ChonkyStation4.exe
   - Click "Browse..." next to "Games Folder" to select your games directory
   - Click "Scan Games" to detect all games

2. **Launching Games**:
   - Double-click any game in the list to launch it
   - Or select a game and click "Launch Selected Game"

3. **Game Detection**:
   - The launcher scans recursively through your games folder
   - It looks for folders containing:
     - `EBOOT.BIN` or `EBOOT.bin` files
     - `.elf` or `.ELF` files
   - Each folder with game files appears as a launchable entry

## How It Works

The launcher uses the command pattern:
```
ChonkyStation4.exe [path-to-game-folder]
```

When you launch a game, it:
1. Starts ChonkyStation4.exe as a subprocess
2. Passes the selected game's folder path as an argument
3. ChonkyStation handles the actual game loading and execution

## Project Structure

```
Chonky GUI/
├── CMakeLists.txt          # CMake configuration
├── main.cpp               # Application entry point
├── chonkylauncher.h       # Main window class header
├── chonkylauncher.cpp     # Main window implementation
├── README.md              # This file
└── ChonkyStation4.exe     # Your ChonkyStation executable
```

## Troubleshooting

- **"ChonkyStation executable not found"**: Make sure you've selected the correct path to ChonkyStation4.exe
- **"No Games Found"**: Ensure your games folder contains subdirectories with EBOOT.BIN or ELF files
- **Build errors**: Verify Qt6 is properly installed and CMake can find it

## License

This launcher is provided as-is for use with ChonkyStation.
