# HydraNes
Simple C++ NES emulator, dependant only on Glew and ImGUI.

## Disclaimer
This emulator is the most barebone possible emulator, mostly a labor of love and curiosity.
Almost no feature present - it is mostly for programmers who wants to understand the internals of the NES emulation.
The code was written with difference of months between check-ins, most of the time at night, thus quality if poor.
All the NES related code is the nes.h/.cpp pair of files.
APU is using the amazing Blargg's code.
ImGUI is used as interface and Glew to use OpenGL code.
Window loop is handmade.


## Build
Built and tested only on Windows, Visual Studio 2015 and 2017.
Just open the solution under project, build and launch.
The roms should be in the folder 'Bin'.

## Compatibility
- NTSC only roms.
- NROM (Mapper 000)
- MMC1 / SxROM (Mapper 001)
- 0UxROM (Mapper 002)
- CNROM (Mapper 003)

## Usage
Run the emulator, click on the windows menu (upper left).
Double-click on cartridge and start the game.

Keys are: 
- up/down/left/right arrows
- enter=start
- shift=select
- z=a
- x=b

## References and credits
- The incredible NesDev wiki and community: http://wiki.nesdev.com/w/index.php/Nesdev.
- Blargg's APU sound chip emulator: http://blargg.8bitalley.com/libs/audio.html#Nes_Snd_Emu
- The amazing ImGui: https://github.com/ocornut/imgui
- The most precise NES emulator: https://github.com/SourMesen/Mesen
