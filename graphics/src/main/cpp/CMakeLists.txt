cmake_minimum_required(VERSION 3.22.1)
project(GraphicsSample LANGUAGES CXX)

find_package(game-activity REQUIRED CONFIG)

add_compile_options(-Wall -Werror -Wextra)

add_library(app
    SHARED
    android_main.cpp
)

target_link_libraries(app
    android
    log
    # Required to force the linker to include this definition in the app, since
    # there's otherwise no reference to it.
    -uJava_com_google_androidgamesdk_GameActivity_initializeNativeCode
    game-activity::game-activity_static
)

target_link_options(app
    PRIVATE
    -flto
    -Wl,--version-script,${CMAKE_SOURCE_DIR}/libapp.map.txt
)

set_target_properties(app
    PROPERTIES
    LINK_DEPENDS ${CMAKE_SOURCE_DIR}/libapp.map.txt
)
