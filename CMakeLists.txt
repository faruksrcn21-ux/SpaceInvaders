cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders)

# C++ Standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML'i DLL olmadan, direkt exe'nin içine gömüyoruz
set(BUILD_SHARED_LIBS OFF)

# SFML'i otomatik indirme bölümü
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

# Kaynak dosyalarımız
add_executable(SpaceInvaders main.cpp GameManager.cpp Player.cpp Enemy.cpp Bullet.cpp Barrier.cpp)

# SFML modüllerini bağla
target_link_libraries(SpaceInvaders sfml-graphics sfml-window sfml-system sfml-audio)