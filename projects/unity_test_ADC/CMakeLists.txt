# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(unity_test_ADC C CXX ASM)

set(CMAKE_C_STANDARD 11)

# Lib com lógica de conversão
add_library(adc_utils
    drivers/adc_utils.c
)

target_include_directories(adc_utils PUBLIC
    include
)

# Testes Unity no host (Linux/PC)
add_executable(test_adc_utils
    tests/test_adc_utils.c
    Unity/src/unity.c
)

target_include_directories(test_adc_utils PRIVATE
    include
    Unity/src
)

target_link_libraries(test_adc_utils
    adc_utils
)

enable_testing()
add_test(NAME adc_test COMMAND test_adc_utils)
