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
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(audio C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Arquivos-fonte do projeto
add_executable(audio
    main.c
    src/audio_capture.c
    src/audio_output.c
    src/io_manager.c
    src/display
    src/ssd1306_i2c.c
)

# Inclui diretórios de cabeçalhos
target_include_directories(audio PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Vincula as bibliotecas padrão do SDK
target_link_libraries(audio
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_i2c
)

# Habilita USB para saída via stdio (opcional)
pico_enable_stdio_usb(audio 1)
pico_enable_stdio_uart(audio 0)

# Gera arquivos UF2, bin, etc.
pico_add_extra_outputs(audio)
