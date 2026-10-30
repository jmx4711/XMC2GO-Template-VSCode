# ---- Werkzeuge ----
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# ---- Projektname ----
TARGET = firmware

# ---- Automatisch alle relevanten Dateien finden ----
# ---- Automatisch alle relevanten Dateien finden ----
SRC      = $(wildcard src/*.c) \
           $(wildcard Libraries/CMSIS/src/*.c) \
		   $(wildcard Libraries/Myown/src/*.c) \
           $(wildcard Libraries/Newlib/src/*.c) \
		   $(wildcard Libraries/XMCLib/src/*.c)

STARTUP  = $(wildcard startup/startup_XMC1100.S)

LINKER   = $(wildcard linker/XMC1100x0064.ld)

# ---- Compiler-Flags ----
CFLAGS  = -mcpu=cortex-m0 -mthumb -Wall -O0 -g3
CFLAGS += -Iinc
CFLAGS += -DXMC1100_Q024x0064

# Libraries
CFLAGS += -ILibraries/CMSIS/inc
CFLAGS += -ILibraries/Myown/inc
CFLAGS += -ILibraries/Newlib/inc
CFLAGS += -ILibraries/XMCLib/inc

# ---- Linker-Flags ----
LDFLAGS = -mcpu=cortex-m0 -mthumb -T$(LINKER) -nostartfiles -Wl,--gc-sections

# ---- Build-Regeln ----
all: $(TARGET).elf $(TARGET).bin

rebuild: clean all

$(TARGET).elf: $(SRC) $(STARTUP)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
ifeq ($(OS),Windows_NT)
	if exist $(TARGET).elf del /Q $(TARGET).elf
	if exist $(TARGET).bin del /Q $(TARGET).bin
else
	rm -f $(TARGET).elf $(TARGET).bin
endif