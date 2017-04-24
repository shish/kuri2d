cmake_minimum_required(VERSION 3.7)
project(kuri2d)

add_definitions(-DVERSION="0.9.1")
set(CMAKE_CXX_STANDARD 11)
link_directories(/opt/local/lib)

set(SOURCE_FILES
        src/editor.c
        src/exit.c
        src/init.c
        src/main.c
        src/kuri2d.h
        src/logic.c
        src/game.c
        src/menu.c
        src/SDL_shish.c
        src/SDL_shish.h src/win.c src/lose.c)

add_executable(kuri2d ${SOURCE_FILES})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})