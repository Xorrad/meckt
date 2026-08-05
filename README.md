# meckt - <span style="font-size: 16px; vertical-align: middle;">a map editor for Crusader Kings 3.</span>

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![GitHub issues](https://img.shields.io/github/issues/xorrad/ck3-meckt)](https://github.com/xorrad/ck3-meckt/issues)
[![GitHub stars](https://img.shields.io/github/stars/xorrad/ck3-meckt.svg?style=flat&label=stars)](https://github.com/xorrad/ck3-meckt)

meckt is a tool whose goal is making total conversion mod easier and less cumbersome. It includes tools to generate provinces from the provinces image, to create and edit titles using a user interface and a map, to manage the history of titles.

> **Warning:**  
> Please use this tool with caution. Exporting modifications will impact significantly the structure and content of your files.  
> Always ensure you have a **backup of your mod files before exporting** to prevent any unintended changes or data loss.  

## Overview

![overview](images/overview-1.8.png)  

![overview](images/overview2-1.8.png)  

![overview](images/overview3-1.8.png)  

![tools](images/tools-1.8.png)  

![views](images/wrapping-1.8.png)  

## Controls

- **Select Multiple Titles:** `Shift + Left Mouse Button (LMB)`.
- **Unselect One Title:** `Ctrl + Left Mouse Button (LMB)`.

- **Unwrap Title:** `Ctrl + Left Mouse Button (LMB)` on the liege title.
- **Wrap Titles Back:** `Right Mouse Button (RMB)` on one of the vassal titles.

## Compiling

### Linux

1. Clone the repository (with submodules):
```bash
git clone --recursive https://github.com/Xorrad/meckt.git
```

2. Install dependencies:
```bash
sudo apt-get update
sudo apt-get install -y build-essential zip pkg-config libgtk-3-dev libcurl4-openssl-dev libopenal-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev libfreetype-dev
```

4. Setup and Compile using CMake:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target meckt-app --config Release
```


### Windows

1. Clone the repository (with submodules):
```bash
git clone --recursive https://github.com/Xorrad/meckt.git
```

2. Install [CMake](https://cmake.org/download/).

3. Setup and Compile using CMake:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target meckt-app --config Release
```

## Contributing

Contributions to the project are highly appreciated! There are several ways to get involved: you can contribute by reporting any issues you encounter, suggesting new features that could enhance the project, or even by actively participating in the development process through the submission of pull requests.

## Third-Party Libraries

- **Simple and Fast Multimedia Library** (https://www.sfml-dev.org/)
- **Dear ImGui** (https://github.com/ocornut/imgui/)
- **ImGui-SFML** (https://github.com/SFML/imgui-sfml)
- **{fmt}** (https://fmt.dev/)
- **Native File Dialog** (https://github.com/mlabbe/nativefiledialog)
- **lodepng** (http://lodev.org/lodepng/)
- **nlohmann-json** (https://github.com/nlohmann/json)
- **jomini-parser** (https://github.com/Xorrad/jomini-parser)
- **doctest** (https://github.com/doctest/doctest)

## License

This project is licensed under the MIT License - see the [LICENSE](https://raw.githubusercontent.com/Xorrad/ck3-meckt/master/LICENSE) file for details.