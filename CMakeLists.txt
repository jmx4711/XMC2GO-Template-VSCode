cmake_minimum_required(VERSION 3.20)

project(XMC2GO C ASM)

set(TARGET firmware)

set(LINKER_SCRIPT
    ${CMAKE_SOURCE_DIR}/linker/XMC1100x0064.ld
)

set(STARTUP_SOURCE
    ${CMAKE_SOURCE_DIR}/startup/startup_XMC1100.S
)

set(SOURCES
    src/main.c

    Libraries/CMSIS/src/system_XMC1100.c # single file in CMSI

    Libraries/Newlib/src/syscalls.c # single file in Newlib

    ${STARTUP_SOURCE}
)

# Automatically grabs all source files in the XMCLib directory
file(GLOB XMCLIB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/Libraries/XMCLib/src/*.c
)

# Automatically grabs all source files in the Myown directory
file(GLOB MYOWN_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/Libraries/Myown/src/*.c
)

# Append the XMCLib and Myown sources to the main SOURCES list
list(APPEND SOURCES
    ${XMCLIB_SOURCES}
    ${MYOWN_SOURCES}
)

add_executable(${TARGET} ${SOURCES})

# Include directories for the target
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Libraries/CMSIS/inc
    ${CMAKE_SOURCE_DIR}/Libraries/Myown/inc
    # ${CMAKE_SOURCE_DIR}/Libraries/Newlib/inc
    ${CMAKE_SOURCE_DIR}/Libraries/XMCLib/inc
)

target_compile_definitions(${TARGET} PRIVATE
    XMC1100_Q024x0064
)

target_compile_options(${TARGET} PRIVATE
    -mcpu=cortex-m0
    -mthumb
    -Wall
    -O0
    -g3
)

target_link_options(${TARGET} PRIVATE
    -mcpu=cortex-m0
    -mthumb
    -T${LINKER_SCRIPT}
    -nostartfiles
    -Wl,--gc-sections
)

set_target_properties(${TARGET} PROPERTIES
    SUFFIX ".elf"
)

add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
            -O binary
            $<TARGET_FILE:${TARGET}>
            ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.bin
    COMMENT "Creating ${TARGET}.bin"
)