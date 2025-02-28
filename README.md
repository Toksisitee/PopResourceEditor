<img align="left" width="100" height="100" src="https://github.com/Toksisitee/PopResourceEditor/blob/master/icon.png">

# PopResourceEditor

Open-source asset editor and manager written in C++ for Bullfrog's Populous: The Beginning, designed to preview, modify, and generate the game's assets.

### Features
  - **Asset Picker**
    - Enter your Populous directory path, and all assets will be automatically detected and displayed in a grid view. Double-click any asset to instantly open it in a new window!
  - **Preview, Modify, Export**
    - Preview assets, generate new ones from scratch, import images (e.g., skies), modify palettes, and export your changes as data files.

### Assets Supported 
For now, the following assets are supported: 
 - **Alpha**: Used for alpha-based effects and sprites.
 - **BigFade**: The primary texture for the landscape.
 - **Blocks**: Texture atlas used by 3D objects.
 - **Cliff**: Landscape texture for damaged ground, lava, and similar effects.
 - **Disp**: Displacement map used to add fine details to the landscape and water texture.
 - **Fade**: Applies shadowing effects to landscape textures, such as shadows or Fog of War.
 - **Ghost**: Used for transparency effects.
 - **Level (v2)**: Map format used to generate detailed map previews.
 - **Palette**: A 256-color table (without an alpha channel), with all assets referencing the indexed palette.
 - **Sky**: Texture for the sky.
 - **Sprite**: 2D textures. GUI, people, spell effects.

### License
PopResourceEditor Copyright © 2024-2025 Toksisitee.
This project is distributed under the MIT License. See the LICENSE file for more details.

### Building
To build PopResourceEditor with Visual Studio 2022, ensure that you have the following dependencies installed:
- **Visual Studio 2022** (v143) with **C++20** (/std:c++20) support.
- **Windows SDK 10.0** (latest version).
- **Precompiled libraries** (e.g., spdlog) are included in the project. However, if you prefer to use your own, you will need to precompile them yourself, as no build tools are provided for third-party libraries.

<p align="center"> <img width="1024" height="960" src="https://github.com/Toksisitee/PopResourceEditor/blob/master/PopResourceEditor.png"></img> </p>
