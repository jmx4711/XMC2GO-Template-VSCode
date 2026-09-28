# ---- Werkzeuge ----
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# ---- Projektname ----
TARGET = firmware

# ---- Automatisch alle relevanten Dateien finden ----
# ---- Automatisch alle relevanten Dateien finden ----
SRC      = $(wildcard src/*.c) \
           $(wildcard Libraries/XMCLib/src/*.c) \
		   $(wildcard Libraries/Newlib/*.c) \
           $(wildcard Libraries/CMSIS/Infineon/COMPONENT_XMC1100/Source/system_XMC1100.c)

STARTUP  = $(wildcard Libraries/CMSIS/Infineon/COMPONENT_XMC1100/Source/TOOLCHAIN_GCC_ARM/startup_XMC1100.S)

LINKER   = $(wildcard linker/XMC1100x0064.ld)

# ---- Compiler-Flags ----
CFLAGS  = -mcpu=cortex-m0 -mthumb -Wall -Os -g
CFLAGS += -Iinc
CFLAGS += -ILibraries/XMCLib/inc
CFLAGS += -ILibraries/CMSIS/Include
CFLAGS += -ILibraries/Newlib
CFLAGS += -ILibraries/CMSIS/Infineon/COMPONENT_XMC1100/Source/TOOLCHAIN_GCC_ARM
CFLAGS += -ILibraries/CMSIS/Infineon/COMPONENT_XMC1100/Include
CFLAGS += -DXMC1100_Q024x0064

# ---- Linker-Flags ----
LDFLAGS = -mcpu=cortex-m0 -mthumb -T$(LINKER) -nostartfiles -Wl,--gc-sections

# ---- Build-Regeln ----
all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(SRC) $(STARTUP)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(TARGET).elf $(TARGET).bin