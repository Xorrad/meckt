# meckt - <span style="font-size: 16px; vertical-align: middle;">a map editor for Crusader Kings 3.</span>

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![GitHub issues](https://img.shields.io/github/issues/xorrad/ck3-meckt)](https://github.com/xorrad/ck3-meckt/issues)
[![GitHub stars](https://img.shields.io/github/stars/xorrad/ck3-meckt.svg?style=flat&label=stars)](https://github.com/xorrad/ck3-meckt)

meckt is a tool whose goal is making total conversion mod easier and less cumbersome. It includes tools to generate provinces from the provinces image, to create and edit titles using a user interface and a map, to manage the history of titles.

> **Warning:**  
> Please use this tool with caution. Exporting modifications will impact significantly the structure and content of your files.  
> Always ensure you have a **backup of your mod files before exporting** to prevent any unintended changes or data loss.  

## Overview

![overview](images/overview.png)  

![wrapping](images/wrapping.png)  

![views](images/views.png)

## Controls

- **Select Multiple Titles:** `Shift + Left Mouse Button (LMB)`.
- **Unselect One Title:** `Ctrl + Left Mouse Button (LMB)`.

- **Unwrap Title:** `Ctrl + Left Mouse Button (LMB)` on the liege title.
- **Wrap Titles Back:** `Right Mouse Button (RMB)` on one of the vassals.

## Compiling

### Linux

1. Clone the repository (with submodules):
```bash
git clone --recursive https://github.com/Xorrad/meckt.git
```

2. Install SFML using the package manager:
```bash
sudo apt-get install libsfml-dev
```

3. In `vendor/includes/imgui/imgui/misc/cpp/imgui_stdlib.cpp` replace line 10 with:
```cpp
#include "../../imgui.h"
```

4. Compile and run *meckt* using:
```bash
make
```


### Windows

1. Clone the repository (with submodules):
```bash
git clone --recursive https://github.com/Xorrad/meckt.git
```

2. Download [SFML 2.5.1](https://www.sfml-dev.org/download/sfml/2.5.1/).  
Extract the downloaded package into the `vendor/sfml/` directory so that the folder structure looks like:
```
vendor/sfml/include
vendor/sfml/lib
...
```

3. Install Make and a compatible C++ compiler for Windows.  
A good starting point is to use the [w64devkit](https://github.com/skeeto/w64devkit), which includes both.

4. In `vendor/includes/imgui/imgui/misc/cpp/imgui_stdlib.cpp` replace line 10 with:
```cpp
#include "../../imgui.h"
```

5. Compile and run *meckt* using:
```bash
make
```

## Contributing

Contributions to the project are highly appreciated! There are several ways to get involved: you can contribute by reporting any issues you encounter, suggesting new features that could enhance the project, or even by actively participating in the development process through the submission of pull requests.

## Third-Party Libraries

- **Simple and Fast Multimedia Library** (https://www.sfml-dev.org/)
- **Dear ImGui** (https://github.com/ocornut/imgui/)
- **ImGui-SFML** (https://github.com/SFML/imgui-sfml)
- **Backward-cpp** (https://github.com/bombela/backward-cpp)
- **{fmt}** (https://fmt.dev/)
- **Native File Dialog** (https://github.com/mlabbe/nativefiledialog)

## License

This project is licensed under the MIT License - see the [LICENSE](https://raw.githubusercontent.com/Xorrad/ck3-meckt/master/LICENSE) file for details.