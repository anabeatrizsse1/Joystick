# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(JOYSTICK C CXX ASM)

pico_sdk_init()

add_executable(JOYSTICK JOYSTICK.c inc/ssd1306/ssd1306.c)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_usb(JOYSTICK 1)
pico_enable_stdio_uart(JOYSTICK 1)

# Add the standard library to the build
target_link_libraries(JOYSTICK
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(JOYSTICK PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries( JOYSTICK
    hardware_adc 
    hardware_pwm
    hardware_i2c
)

pico_add_extra_outputs(JOYSTICK)