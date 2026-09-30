# XMC2GO-Template-VSCode

1) Install VSCode
2) Install VSCode extensions: Intellisense for C/C++, Cortex-Debugger
3) Install arm gnu chain (for macOS)

brew install gcc-arm-embedded

4) Import Template into own folder using git:

git clone https://github.com/jmx4711/XMC2GO-Template-VSCode

5) Remove .gitignore in folder to remove connection to repository.


// Test JLink Connection:

JLinkExe -device XMC1100-0064 -if SWD -speed 4000 -autoconnect 1