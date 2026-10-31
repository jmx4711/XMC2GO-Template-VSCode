# XMC2GO-Template-VSCode

Installation guide (for build system with CMake and Ninja for build speed optimization)

1) Install VSCode

2) Install VSCode extensions: Intellisense for C/C++, Cortex-Debugger, CMake

3) Install terminal tools 

    MacOS:
    `brew install gcc-arm-embedded`
    `brew install cmake`
    `brew install ninja`

    Windows:
    `winget install gcc-arm-none-eabi`
    `winget install cmake`
    `winget install Ninja-build.Ninja`

4) Import Template into own folder using git:
    `git clone https://github.com/jmx4711/XMC2GO-Template-VSCode`

5) Remove .gitignore in folder to remove connection to repository.

    MacOS:
    `rm -f ".gitignore" ".git"`

    Windows:
    `del ".gitignore" ".git"`

6) Install Segger JLink
    
    Mac:
    `brew install segger-jlink`

    All:
    via website: https://www.segger.com/downloads/jlink/

7) Test JLink Connection:
    
    `JLinkExe -device XMC1100-0064 -if SWD -speed 4000 -autoconnect 1`

    or using J-Link Commander

8) Have fun with it :D
