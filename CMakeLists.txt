# Standard routine
cmake_minimum_required(VERSION 3.16)
project(telegram_client)

set(TELEGRAM_API_ID "" CACHE STRING "telegram api id")
set(TELEGRAM_API_HASH "" CACHE STRING "telegram api hash")

if(NOT TELEGRAM_API_ID)
    message(FATAL_ERROR "TELEGRAM_API_ID is mandatory.")
endif()

if(NOT TELEGRAM_API_HASH)
    message(FATAL_ERROR "TELEGRAM_API_HASH is mandatory.")
endif()

set(AUI_VERSION 403a268f13bf6649294945b72ec0003bb65a5f68)

set(BUILD_SHARED_LIBS OFF)

# Use AUI.Boot
file(
        DOWNLOAD
        https://raw.githubusercontent.com/aui-framework/aui/${AUI_VERSION}/aui.boot.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/aui.boot.cmake)
include(${CMAKE_CURRENT_BINARY_DIR}/aui.boot.cmake)

# import AUI
auib_import(aui https://github.com/aui-framework/aui
        COMPONENTS core views
        VERSION ${AUI_VERSION})

# import tdlib (telegram client library)
auib_import(Td https://github.com/tdlib/td
        VERSION 18618ca)

# Create the executable. This function automatically links all sources from the src/ folder,
# creates CMake target and places the resulting executable to bin/ folder.
aui_executable(${PROJECT_NAME})

# Link required libs
aui_link(${PROJECT_NAME} PRIVATE aui::core aui::views Td::TdStatic)

target_compile_definitions(${PROJECT_NAME} PRIVATE TELEGRAM_API_ID=${TELEGRAM_API_ID} TELEGRAM_API_HASH=${TELEGRAM_API_HASH})

aui_compile_assets(${PROJECT_NAME})
aui_enable_tests(${PROJECT_NAME})
